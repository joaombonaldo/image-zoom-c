#include <stdio.h>
#include <stdlib.h>
#include "lib_ppm.h"

// Função para calcular os sub-pixels
void calcular_subpixel(struct pixel_s *src_pixel, struct pixel_s *sub_pixels) {
    unsigned char r = src_pixel->r;
    unsigned char g = src_pixel->g;
    unsigned char b = src_pixel->b;

    // Inicializa todos os sub-pixels como preto
    for (int i = 0; i < 9; i++) {
        sub_pixels[i].r = 0;
        sub_pixels[i].g = 0;
        sub_pixels[i].b = 0;
    }

    // Ajustando os níveis de intensidade para ativar os sub-pixels
    if (r > 100) {
        // Se o vermelho é intenso, ativar sub-pixels com diferentes padrões
        sub_pixels[1].r = r;
        sub_pixels[4].r = r;
        sub_pixels[7].r = r;
    }
    if (g > 100) {
        // Se o verde é intenso, ativar outros padrões de sub-pixels
        sub_pixels[3].g = g;
        sub_pixels[4].g = g;
        sub_pixels[5].g = g;
    }
    if (b > 100) {
        // Se o azul é intenso, ativar outros padrões de sub-pixels
        sub_pixels[0].b = b;
        sub_pixels[4].b = b;
        sub_pixels[8].b = b;
    }
}

// Função para aplicar o zoom e o processamento de sub-pixel
void aplicar_zoom(struct image_s *src, struct image_s *dest) {
    for (int y = 0; y < src->height; y++) {
        for (int x = 0; x < src->width; x++) {
            struct pixel_s sub_pixels[9];
            calcular_subpixel(&src->pix[y * src->width + x], sub_pixels);

            // Copiar os sub-pixels para a imagem destino
            for (int dy = 0; dy < 3; dy++) {
                for (int dx = 0; dx < 3; dx++) {
                    int dest_x = x * 3 + dx;
                    int dest_y = y * 3 + dy;
                    dest->pix[dest_y * dest->width + dest_x] = sub_pixels[dy * 3 + dx];
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <imagem_entrada.ppm> <imagem_saida.ppm>\n", argv[0]);
        return 1;
    }

    struct image_s src, dest;
    // Lendo a imagem de entrada
    if (read_ppm(argv[1], &src) != 0) {
        fprintf(stderr, "Erro ao ler a imagem %s\n", argv[1]);
        return 1;
    }

    // Criando a imagem de destino com o novo tamanho (3x maior)
    if (new_ppm(&dest, src.width * 3, src.height * 3) != 0) {
        fprintf(stderr, "Erro ao criar a imagem de destino\n");
        free_ppm(&src);
        return 1;
    }

    // Aplicando o zoom na imagem
    aplicar_zoom(&src, &dest);

    // Escrevendo a imagem de saída
    if (write_ppm(argv[2], &dest) != 0) {
        fprintf(stderr, "Erro ao escrever a imagem %s\n", argv[2]);
        free_ppm(&src);
        free_ppm(&dest);
        return 1;
    }

    // Liberando memória alocada para as imagens
    free_ppm(&src);
    free_ppm(&dest);

    return 0;
}
