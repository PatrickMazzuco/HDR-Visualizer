#include <math.h>
#include <string.h>		// para usar strings

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Rotinas para leitura de arquivos .hdr
#include "rgbe.h"

// Variáveis globais a serem utilizadas:

// Dimensões da imagem de entrada
int sizeX, sizeY;

// Imagem de entrada
RGBf* image;

// Imagem de saída
RGB8* image8;

// Fator de exposição inicial
float exposure = 1.0;

// Modo de exibição atual
int modo;

// Função pow mais eficiente (cerca de 7x mais rápida)
float fastpow(float a, float b);
float fastpow(float a, float b) {
     union { float f; int i; }
      u = { a };
      u.i = (int)(b * (u.i - 1065307417) + 1065307417);
      return u.f;
}

//Converter para 25 Bits
float to24Bit(float in) {
return fmin(1.0, in) * 255;
}

// Função principal de processamento: ela deve chamar outras funções
// quando for necessário (ex: algoritmos de tone mapping, etc)
void process()
{
    if (exposure > 1.0) exposure = 1.0;
    printf("Exposure: %.3f\n", exposure);

    float tempR, tempG, tempB;

    //Scale
    if (modo == 0) {
        float c = 0.3;
        for (int i = 0; i < sizeX*sizeY; i++) {
            tempR = image[i].r/(image[i].r + c);
            tempG = image[i].g/(image[i].g + c);
            tempB = image[i].b/(image[i].b + c);

            //Adicionar ao array de saida.
            image8[i].r = (unsigned char) to24Bit(tempR) * exposure;
            image8[i].g = (unsigned char) to24Bit(tempG) * exposure;
            image8[i].b = (unsigned char) to24Bit(tempB) * exposure;

        }
    } else {
        //Gamma
        if (modo == 1) {
            float gammaCorrection = 2.2;
            for (int i = 0; i < sizeX*sizeY; i++) {
                tempR = fastpow(image[i].r, 1/gammaCorrection);
                tempG = fastpow(image[i].g, 1/gammaCorrection);
                tempB = fastpow(image[i].b, 1/gammaCorrection);

                //Adicionar ao array de saida.
                image8[i].r = (unsigned char) to24Bit(tempR) * exposure;
                image8[i].g = (unsigned char) to24Bit(tempG) * exposure;
                image8[i].b = (unsigned char) to24Bit(tempB) * exposure;
            }
        }
    }

    //
    // NÃO ALTERAR A PARTIR DAQUI!!!!
    //
    buildTex();
}

//Recebe o nome do arquivo como argumento.
int main(int argc, char** argv)
{
    if(argc==1) {
        printf("hdrvis [image file.hdr]\n");
        exit(1);
    }

    // Inicialização da janela gráfica
    init(argc,argv);

    //Ler imagem de entrada:

    FILE* arq = fopen(argv[1],"rb");

    //Se o arquivo foi encontrado:
    if (arq != NULL) {
        RGBE_ReadHeader(arq, &sizeX, &sizeY, NULL);
        image = (RGBf*) malloc(sizeof(RGBf) * sizeX * sizeY);
        int result = RGBE_ReadPixels_RLE(arq, (float*)image, sizeX, sizeY);
        fclose(arq);

        printf("%d x %d\n", sizeX, sizeY);

        exposure = 1.0f; //Exposicao inicial

        //Array de saida
        image8 = (RGB8*) malloc(sizeof(RGB8) * sizeX * sizeY);

        process();
        glutMainLoop();
    } else {
        printf("File %s not found.", argv[1]);
        exit(2);
    }

    return 0;
}

