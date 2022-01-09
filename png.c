#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"
#include "crc.h"

uint8_t crc_startup = 0;
uint32_t check_crc(Chunk * chunk) {
    if (!crc_startup) {
        crcInit();
        crc_startup = 1;
    }

    int32_t new_crc = crcFast(chunk->data, chunk->lenght);
    printf("%u == %u ? %u\n", new_crc, chunk->crc, new_crc == chunk->crc);
    return new_crc == chunk->crc;
}


const char PNG_sign [] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0};
bool is_PNG(FILE * image){

    // Criando variável para guardar a assinatura
    Byte signature[9];
    signature[8] = 0; /* Caractere vazio para in-
                         dicar fim da string. */
    // Lendo os 8 bytes da imagem e colocando na variavel signature
    fread(signature, 1, 8, image);
    // Checando se o arquivo é ou não uma PNG
    if (strcmp((const char *)signature, PNG_sign) == 0)
        return  true;
    return false;
}

void fwrite_sign(FILE * outfile) {
    fputs(PNG_sign, outfile);
}

void correct_litle_endian(Byte * bytes){
    char reverse[4];
    int i, r;
    for (i = 0, r = 3; i < 4 && r >= 0; i++, r--){
        reverse[i] = bytes[r];
    }
    for (i = 0; i < 4; i++)
        bytes[i] = reverse[i];
}

Chunk * next_chunk(FILE * image){
    Chunk* chunk = (Chunk*)malloc(sizeof (Chunk));

    // Lendo o lenght
    fread(&chunk->lenght, 1, 4, image);

    // Corrigindo bug de little endian
    Byte * lenght = (Byte *)&chunk->lenght;
    correct_litle_endian(lenght);

    // Tem que vir antes do data, para não corromper os dados do mesmo
    fread(chunk->type, 4, 1, image);
    chunk->type[4] = 0;

    // Lendo os dados do chunk
    chunk->data = (Byte *)malloc(chunk->lenght);

    for (int i = 0; i < chunk->lenght; i++)
        chunk->data[i] = fgetc(image);

    // Lendo o crc
    fread(&chunk->crc, 4, 1, image);     // Salvando dados no cabeçalho

    // Corrigindo bug de little endian
    Byte * crc = (Byte *)&chunk->crc;
    correct_litle_endian(crc);

    return chunk;
}

void fwrite_chunk(FILE * outfile, Chunk * chunk) {
    int orig_lenght = chunk->lenght;

    Byte * lenght = (Byte *)&chunk->lenght;
    correct_litle_endian(lenght);

    // chunk->crc = check_crc(chunk) ?crcFast(chunk->data, chunk->lenght) :chunk->crc;

    Byte * crc = (Byte *)&chunk->crc;
    correct_litle_endian(crc);

    fwrite(lenght, 1, 4, outfile);
    fwrite(chunk->type, 1, 4, outfile);
    fwrite(chunk->data, 1, orig_lenght, outfile);
    fwrite(crc, 1, 4, outfile);

    correct_litle_endian(lenght);
    correct_litle_endian(crc);
}

void trash_chunk(Chunk * chunk){
    free(chunk->data);
    free(chunk);
}

IHDR* to_IHDR(Byte * raw_data){
    Dimentions* data = (Dimentions*)raw_data;
    correct_litle_endian(data->width);
    correct_litle_endian(data->height);
    IHDR* header = (IHDR*)raw_data;
    return header;
}
