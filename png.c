#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"

bool is_PNG(FILE * image){
    const char PNG_sign [] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0};

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
    Chunk* block = (Chunk*)malloc(sizeof (Chunk));
    fread(block, 8, 1, image);

    // Corrigindo bug de little endian
    Byte * lenght = (Byte *)block;
    correct_litle_endian(lenght);

    // Tem que vir antes do data, para não corromper os dados do mesmo
    block->type[4] = 0;
    block->data = (Byte *)malloc(block->lenght);

    // Lendo os dados do chunk
    fread(block->data, block->lenght, 1, image);

    // Lendo o crc
    fread(&block->crc, 4, 1, image);     // Salvando dados no cabeçalho
    return block;
}

void fput_bytes(FILE * outfile, Byte * b, size_t size) {
    printf("%li \n", size);
    for (int i = 0; i < size; i++)
        fputc(b[i], outfile);
}

void fwrite_chunk(FILE * outfile, Chunk * chunk) {
    int orig_lenght = chunk->lenght;

    Byte * lenght = (Byte *)chunk;
    correct_litle_endian(lenght);

    fwrite(lenght, 1, 4, outfile);
    fput_bytes(outfile, chunk->type, 4);
    fput_bytes(outfile, chunk->data, orig_lenght);
    fputc(chunk->crc, outfile);

}

void trash_chunk(Chunk * block){
    free(block->data);
    free(block);
}

IHDR* to_IHDR(Byte * raw_data){
    Dimentions* data = (Dimentions*)raw_data;
    correct_litle_endian(data->width);
    correct_litle_endian(data->height);
    IHDR* header = (IHDR*)raw_data;
    return header;
}
