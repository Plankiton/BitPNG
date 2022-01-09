#include <stdint.h> // Para usarmos o int32_t
#include <stdio.h>
#include <stdbool.h>
typedef unsigned char Byte;
// Assinatura :        89    50    4e    47    0d    0a    1a    0a  

typedef struct {
    int32_t lenght; // O int 32 sempre tem 4 bytes
    Byte   type[5]; // O tipo é sempre uma string
    Byte *    data; // Os dados tem tamanho variável
    int32_t    crc;
} Chunk;

typedef enum {
    GrayScale = 0,
    RGB = 2,
    Pallete = 3,
    GrayScaleAlpha = 4,
    RGBAlpha = 6
} ColorType;

typedef enum {
    NoInterlace = 0,
    Adam7 = 1
} Interlace;

typedef struct {
    uint32_t          width; // Comprimento
    uint32_t         height; // Altura
    Byte              depth; // Profundidade de bits
    Byte              color; // Tipo de cor
    Byte          interlace; // Tipo de intrelaçamento
    Byte             filter; // Tipo de cor
    Byte        compression; // Tipo de compressão
} IHDR;

typedef struct {
    Byte   width[4]; // Comprimento
    Byte  height[4]; // Altura
} SIZE_RAW;
typedef SIZE_RAW Dimentions;

bool is_PNG(FILE * image);
void fwrite_sign(FILE * outfile);
void correct_litle_endian(Byte * bytes);
Chunk * next_chunk(FILE * image);
void fwrite_chunk(FILE * outfile, Chunk * chunk);
void trash_chunk(Chunk * block);
IHDR* to_IHDR(Byte * raw_data);
