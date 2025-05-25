//
//
// Created by abiga on 27/04/2025.
//

#ifndef BMP8_H
#define BMP8_H

#include <stdint.h>
#include <stdio.h>

//1.1 The t_bmp8 structure
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

//1.2 Reading and Writing an 8-bit Grayscale Image
t_bmp8 * bmp8_loadImage(const char * filename);
void bmp8_saveImage(const char *filename, const t_bmp8 * img);
void bmp8_free(t_bmp8 * img);
void bmp8_printInfo(const t_bmp8 * img);

//1.2.2 The bmp8_saveImage function
//1.2.3 The bmp8_free function
//1.2.4 The bmp8_printInfo function


//1.3 Image Processing Functions
void bmp8_negative(t_bmp8 * img);
void bmp8_brightness(t_bmp8 * img, int value);
void bmp8_threshold(t_bmp8 * img, int threshold);

//Kernel op
void freekernel(float** kernel,int size);
float** allocateKernel(int size);

//1.4 Image filtering
void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);
void bmp8_boxBlur(t_bmp8 * img);
void bmp8_gaussianBlur(t_bmp8 * img);
void bmp8_outline(t_bmp8 * img);
void bmp8_emboss(t_bmp8 * img);
void bmp8_sharpen(t_bmp8 * img);


#endif //BMP8_H
