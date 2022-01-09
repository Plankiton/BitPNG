#include <string.h> // strcmp
#include <stdio.h>  // FILE, fopen
#include <stdlib.h> // exit
#include "png.h"    // Chunk
#define MAX_FILENAME_SIZE 255

void die(char *);

Chunk * bloco, * idat;
void defer(){
    if (idat)
        trash_chunk(idat);
    if (bloco)
        trash_chunk(bloco);
}

int main(int c, char ** a){
    char outfilename[MAX_FILENAME_SIZE];
    strcpy(outfilename, a[1]);
    strcat(outfilename, ".new");

    // Criando o arquivo de ouput da imagem filtrada
    FILE * outfile = fopen(outfilename, "wb");
    // Escreve signature na imagem final
    fwrite_sign(outfile);

    // Criando o arquivo e abrindo a imagem
    FILE * image = fopen(a[1], "rb");
    if (!image)
        die("Não foi possível ler a imagem");
    if (!is_PNG(image))
        die("A imagem não é uma PNG");

    // Pegando primeiro Chunk
    bloco = next_chunk(image);
    if (strcmp((const char*)bloco->type, "IHDR"))
        die("Não foi possível ler o cabeçalho do arquivo");

    // Escreve chunk de cabeçalho
    fwrite_chunk(outfile, bloco);

    // Convertendo Chunk para um cabeçalho
    IHDR * cabecalho = to_IHDR(bloco->data);
    printf("Tamanho do cabecalho: %i, Largura: %i, Altura: %i, Tipo de cor: %i\n",
            bloco->lenght, cabecalho->height, cabecalho->width, cabecalho->color);

    // Avisando se a imagem não for suportada pelo programa
    if (cabecalho->color != RGB)
        die("Formato de cores não suportado, deve ser RGB");
    if (cabecalho->interlace)
        die("Imagem não pode conter entrelaçamento");
    if (cabecalho->filter)
        die("Imagem não pode conter filtro");
    if (cabecalho->compression)
        die("Imagem não pode conter compressão");

    // Enquanto o chunk não for nulo e diferente de IDAT
    while ((idat = next_chunk(image)) && strcmp((const char*)idat->type, "IDAT")){
        // fwrite_chunk(outfile, idat);

        // Como o chunk não é um IDAT ele deve ser limpo antes de buscarmos o próximo
        trash_chunk(idat);
    }

    // Enquanto o chunk não for nulo e diferente de IEND (ou seja, enquanto o chunk é um IDAT)
    int i = 0;
    do {

        Byte * data = idat->data;

        for (int j = 0; j+2 < idat->lenght; j+=3) {
            printf("%i%i%i", data[j+0], data[j+1], data[j+2]);
            /* printf("%02X%02X%02X ", data[j+0], data[j+1], data[j+2]);
            if (j+1%((int)cabecalho->width) == 0)
                puts("");*/

            int average = (data[j+0] + data[j+1] + data[j+2]) / 3;
            data[j+0] = average;
            data[j+0] = average;
            data[j+0] = average;

            i++;
        }

        fwrite_chunk(outfile, idat);
        trash_chunk(idat);
    } while ((idat = next_chunk(image)) && strcmp((const char*)idat->type, "IEND"));

    fwrite_chunk(outfile, idat);
    if (idat)
        trash_chunk(idat);

    fclose(image);
    fclose(outfile);
    return 0;
}

// Função que interrompe o programa e exibe mensagem em caso de erros
void die(char * msg){
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}
