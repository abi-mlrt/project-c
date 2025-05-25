//
// Created by abiga on 27/04/2025.
//

#ifndef BMP24_H
#define BMP24_H
#include <stdlib.h>
#include <stdio.h>

#include <stdint.h>

//2.2.1 The t_bmp_header and t_bmp_info data types
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;

//2.2.2 The t_pixel data type
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;

//2.2 The t_bmp24 data type
typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;




//2.3 Allocation and deallocation functions

t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);

//2.4 Features: Loading and Saving 24-bit Images
t_bmp24 * bmp24_loadImage (const char * filename);
void bmp24_saveImage (t_bmp24 * img, const char * filename);

//2.4.2 Read and write pixel data
void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_readPixelData (t_bmp24 * image, FILE * file);
void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
void bmp24_writePixelData (t_bmp24 * image, FILE * file);

//2.4.3 The bmp24_loadImage function
t_bmp24* bmp24_loadImage (const char * filename);
//2.4.4 The bmp24_saveImage function
void bmp24_saveImage (t_bmp24 * img, const char * filename);

//2.5 Features: 24-bit Image Processing
void bmp24_negative (t_bmp24 * img);
void bmp24_grayscale (t_bmp24 * img);
void bmp24_brightness (t_bmp24 * img, int value);

//2.6 Features: Convolution Filters
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize);
void bmp24_boxBlur (t_bmp24 * img);
void bmp24_gaussianBlur (t_bmp24 * img);
void bmp24_outline (t_bmp24 * img);
void bmp24_emboss (t_bmp24 * img);
void bmp24_sharpen (t_bmp24 * img);

//3. equalization

typedef struct {
    float Y;  // Luminance (0-255)
    float U;  // Chrominance (-127 to 127)
    float V;  // Chrominance (-127 to 127)
} t_yuv;

void bmp24_equalize(t_bmp24 *img);
void bmp24_histogramEqualization (t_bmp24 *img);
t_yuv rgb_to_yuv(t_pixel rgb);
t_pixel yuv_to_rgb(t_yuv yuv);
unsigned int* bmp24_computeLuminanceHistogram(t_bmp24 *img);
unsigned int* bmp24_computeNormalizedCDF(unsigned int* hist, int total_pixels);
#endif //BMP24_H
