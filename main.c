#include <string.h> // strcmp
#include <stdio.h>  // FILE, fopen
#include <stdlib.h> // exit
#include "png.h"    // Chunk

void die(char *);

int main(){
    // Criando o arquivo e abrindo a imagem
    FILE * image = fopen("acdc.min.png", "rb");
    if (!image)
        die("Não foi possível ler a imagem");
    if (!is_PNG(image))
        die("A imagem não é uma PNG");

    // Pegando primeiro Chunk
    Chunk * bloco = next_chunk(image);
    if (strcmp(bloco->type, "IHDR"))
        die("Não foi possível ler o cabeçalho do arquivo");

    // Convertendo Chunk para um cabeçalho
    IHDR * cabecalho = to_IHDR(bloco->data);
    printf("Tamanho do cabecalho: %i, Largura: %i, Altura: %i\n",
            bloco->lenght, cabecalho->height, cabecalho->width);
    trash_chunk(bloco);
    return 0;
}

// Função que interrompe o programa e exibe mensagem em caso de erros
void die(Byte * msg){
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}
