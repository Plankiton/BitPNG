#include "png.h"
#include "crc.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char PNG_sign[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0};
bool is_PNG(FILE *image) {

  // Criando variável para guardar a assinatura
  Byte signature[9];
  signature[8] = 0; /* Caractere vazio para in-
                       dicar fim da string. */
  // Lendo os 8 bytes da imagem e colocando na variavel signature
  fread(signature, 1, 8, image);
  // Checando se o arquivo é ou não uma PNG
  if (strcmp((const char *)signature, PNG_sign) == 0)
    return true;
  return false;
}

void fwrite_sign(FILE *outfile) { fputs(PNG_sign, outfile); }

void correct_litle_endian(Byte *bytes) {
  char reverse[4];
  int i, r;
  for (i = 0, r = 3; i < 4 && r >= 0; i++, r--) {
    reverse[i] = bytes[r];
  }
  for (i = 0; i < 4; i++)
    bytes[i] = reverse[i];
}

Chunk *next_chunk(FILE *image) {
  Chunk *chunk = (Chunk *)malloc(sizeof(Chunk));

  // Lendo o lenght
  fread(&chunk->lenght, 1, 4, image);

  // Corrigindo bug de little endian
  Byte *lenght = (Byte *)&chunk->lenght;
  correct_litle_endian(lenght);

  // Tem que vir antes do data, para não corromper os dados do mesmo
  fread(chunk->type, 4, 1, image);
  chunk->type[4] = 0;

  // Lendo os dados do chunk
  chunk->data = (Byte *)malloc(chunk->lenght);

  for (int i = 0; i < chunk->lenght; i++)
    chunk->data[i] = fgetc(image);

  // Lendo o crc
  fread(&chunk->crc, 4, 1, image); // Salvando dados no cabeçalho

  // Corrigindo bug de little endian
  Byte *crc = (Byte *)&chunk->crc;
  correct_litle_endian(crc);

  return chunk;
}

void fwrite_chunk(FILE *outfile, Chunk *chunk) {
  size_t crc_data_len = chunk->lenght + 4;

  Byte *data = malloc(crc_data_len);
  memcpy(data, chunk->type, 4);
  memcpy(data + 4, chunk->data, chunk->lenght);

  Byte *len_bytes = malloc(4);
  memcpy(len_bytes, (Byte *)&chunk->lenght, 4);

  int new_crc = crc32(0x0L, data, crc_data_len); // ^ 0xFFL;
  printf("%s: crc %.4x -> %.4x\n", chunk->type, chunk->crc, new_crc);

  chunk->crc = new_crc;
  Byte *crc_bytes = malloc(4);
  memcpy(crc_bytes, (Byte *)&chunk->crc, 4);

  correct_litle_endian(crc_bytes);
  correct_litle_endian(len_bytes);
  fwrite(len_bytes, 1, 4, outfile);
  fwrite(data, 1, crc_data_len, outfile);
  fwrite(crc_bytes, 1, 4, outfile);
  free(data);
}

void trash_chunk(Chunk *chunk) {
  free(chunk->data);
  free(chunk);
}

IHDR *to_IHDR(Byte *raw_data) {
  Dimentions *data = (Dimentions *)raw_data;
  correct_litle_endian(data->width);
  correct_litle_endian(data->height);
  IHDR *header = (IHDR *)raw_data;
  return header;
}
