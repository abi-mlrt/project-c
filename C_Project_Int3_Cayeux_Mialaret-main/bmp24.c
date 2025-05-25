//
// Created by abiga on 27/04/2025.
//

#include "bmp24.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Offsets for the BMP header
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28
#define BITMAP_SIZE_RAW 0x22 // offset 34
// Magical number for BMP files
#define BMP_TYPE 0x4D42 // 'BM' in hexadecimal
// Header sizes
#define HEADER_SIZE 0x0E // 14 octets
#define INFO_SIZE 0x28 // 40 octets
// Constant for the color depth
#define DEFAULT_DEPTH 0x18 // 24
//2.2

//2.3 Allocation and deallocation functions
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    // Allocation of row pointers
    t_pixel **pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (!pixels) {
        return NULL;
    }

    // allocation of each row
    for (int i = 0; i < height; i++) {
        pixels[i] = (t_pixel *)malloc(width * sizeof(t_pixel));
        if (!pixels[i]) {
            // In case the allocation fails
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }

    return pixels;
}
void bmp24_freeDataPixels (t_pixel ** pixels, int height) {
    if (pixels == NULL) {return;}
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}
t_bmp24* bmp24_allocate(int width, int height, int colorDepth){
    if(width <= 0 || height <= 0 || colorDepth != 24) return NULL;
    t_bmp24 *img = (t_bmp24*)malloc(sizeof(t_bmp24));
    if(!img) return NULL;
    img->data = bmp24_allocateDataPixels(width, height);
    if(!img->data){
        free(img);
        return NULL;
    }
    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->header.type = 0x4D42;
    img->header.offset = sizeof(t_bmp_header) + sizeof(t_bmp_info);
    img->header.size = img->header.offset + (width * height * 3);
    img->header.reserved1 = 0;
    img->header.reserved2 = 0;
    img->header_info.size = sizeof(t_bmp_info);
    img->header_info.width = width;
    img->header_info.height = height;
    img->header_info.planes = 1;
    img->header_info.bits = colorDepth;
    img->header_info.compression = 0;
    img->header_info.imagesize = width * height * 3;
    img->header_info.xresolution = 0;
    img->header_info.yresolution = 0;
    img->header_info.ncolors = 0;
    img->header_info.importantcolors = 0;
    return img;
}
void bmp24_free(t_bmp24 *img){
    if(img == NULL){
        return;
    }
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}
//2.4 Features: Loading and Saving 24-bit Images
//2.4.1. Assisting functions
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file){
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file){
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


//2.4.2 Read and write pixel data
void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file){
    long offset = image->header.offset + ((image->height - 1 - y) * image->width + x) * 3;
    fseek(file, offset, SEEK_SET);
    unsigned char bgr[3];
    fread(bgr, sizeof(unsigned char), 3, file);
    image->data[y][x].blue = bgr[0];
    image->data[y][x].green = bgr[1];
    image->data[y][x].red = bgr[2];
}
void bmp24_readPixelData(t_bmp24 *image, FILE *file){
    for(int y = 0; y < image->height; y++){
        for(int x = 0; x < image->width; x++){
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}
void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file){
    long offset = image->header.offset + ((image->height - 1 - y) * image->width + x) * 3;
    fseek(file, offset, SEEK_SET);

    unsigned char bgr[3];
    bgr[0] = image->data[y][x].blue;
    bgr[1] = image->data[y][x].green;
    bgr[2] = image->data[y][x].red;

    fwrite(bgr, sizeof(unsigned char), 3, file);
}
void bmp24_writePixelData(t_bmp24 *image, FILE *file){
    for(int y = 0; y < image->height; y++){
        for(int x = 0; x < image->width; x++){
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}

//2.4.3 The bmp24_loadImage function
t_bmp24 *bmp24_loadImage(const char *filename){
    FILE *file = fopen(filename, "rb");
    if(!file){
        printf("Couldn't open %s :(\n", filename);
        return NULL;
    }

    int width, height;
    short depth;

    file_rawRead(BITMAP_WIDTH, &width, sizeof(int), 1, file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(int), 1, file);
    file_rawRead(BITMAP_DEPTH, &depth, sizeof(short), 1, file);

    if(depth != 24){
        printf("Only 24-bit BMP images can be used :(\n");
        fclose(file);
        return NULL;
    }

    t_bmp24 *img = bmp24_allocate(width, height, depth);
    if(!img){
        printf("Couldn't allocate memory for image :(\n");
        fclose(file);
        return NULL;
    }
    file_rawRead(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawRead(BITMAP_WIDTH - HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);
    bmp24_readPixelData(img, file);
    fclose(file);
    return img;
}
//2.4.4 The bmp24_saveImage function
void bmp24_saveImage(t_bmp24 *img, const char *filename){
    FILE *file = fopen(filename, "wb");
    if(!file){
        printf("Couldn't open %s\n", filename);
        return;
    }
    file_rawWrite(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawWrite(BITMAP_WIDTH - HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);
    bmp24_writePixelData(img, file);
    fclose(file);
}

//2.5 Features: 24-bit Image Processing
void bmp24_negative(t_bmp24 *img){
    for(int y = 0; y < img->height; y++){
        for(int x = 0; x < img->width; x++){
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 *img){
    for(int y = 0; y < img->height; y++){
        for(int x = 0; x < img->width; x++){
            int avg = (img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue) / 3;
            img->data[y][x].red = avg;
            img->data[y][x].green = avg;
            img->data[y][x].blue = avg;
        }
    }
}
void bmp24_brightness(t_bmp24 *img, int value){
    for(int y = 0; y < img->height; y++){
        for(int x = 0; x < img->width; x++){
            int r = img->data[y][x].red + value;
            int g = img->data[y][x].green + value;
            int b = img->data[y][x].blue + value;

            img->data[y][x].red = r > 255 ? 255 : (r < 0 ? 0 : r);
            img->data[y][x].green = g > 255 ? 255 : (g < 0 ? 0 : g);
            img->data[y][x].blue = b > 255 ? 255 : (b < 0 ? 0 : b);
        }
    }
}

//2.6 Features: Convolution Filters
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize){}
void bmp24_boxBlur (t_bmp24 * img){}
void bmp24_gaussianBlur (t_bmp24 * img){}
void bmp24_outline (t_bmp24 * img){}
void bmp24_emboss (t_bmp24 * img){}
void bmp24_sharpen (t_bmp24 * img){}





// Convert RGB pixel to YUV
t_yuv rgb_to_yuv(t_pixel rgb) {
    t_yuv yuv;
    yuv.Y = 0.299f * rgb.red + 0.587f * rgb.green + 0.114f * rgb.blue;
    yuv.U = -0.14713f * rgb.red - 0.28886f * rgb.green + 0.436f * rgb.blue;
    yuv.V = 0.615f * rgb.red - 0.51499f * rgb.green - 0.10001f * rgb.blue;
    return yuv;
}

// Convert YUV back to RGB
t_pixel yuv_to_rgb(t_yuv yuv) {
    t_pixel rgb;

    // Apply conversion formulas
    float r = yuv.Y + 1.13983f * yuv.V;
    float g = yuv.Y - 0.39465f * yuv.U - 0.58060f * yuv.V;
    float b = yuv.Y + 2.03211f * yuv.U;

    // Clamp values to 0-255 range
    rgb.red = (uint8_t)fmaxf(0, fminf(255, roundf(r)));
    rgb.green = (uint8_t)fmaxf(0, fminf(255, roundf(g)));
    rgb.blue = (uint8_t)fmaxf(0, fminf(255, roundf(b)));

    return rgb;
}

// Function to equalize a color image in YUV space
void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) return;

    // Step 1: Convert to YUV and collect Y values
    float *Y_values = (float *)malloc(img->width * img->height * sizeof(float));
    t_yuv **yuv_img = (t_yuv **)malloc(img->height * sizeof(t_yuv *));

    if (!Y_values || !yuv_img) {
        if (Y_values) free(Y_values);
        if (yuv_img) free(yuv_img);
        return;
    }

    for (int i = 0; i < img->height; i++) {
        yuv_img[i] = (t_yuv *)malloc(img->width * sizeof(t_yuv));
        if (!yuv_img[i]) {
            // Cleanup already allocated memory
            for (int j = 0; j < i; j++) free(yuv_img[j]);
            free(yuv_img);
            free(Y_values);
            return;
        }

        for (int j = 0; j < img->width; j++) {
            yuv_img[i][j] = rgb_to_yuv(img->data[i][j]);
            Y_values[i * img->width + j] = yuv_img[i][j].Y;
        }
    }

    // Step 2: Compute histogram of Y channel
    unsigned int hist[256] = {0};
    for (int i = 0; i < img->width * img->height; i++) {
        int y_val = (int)round(Y_values[i]);
        y_val = y_val < 0 ? 0 : (y_val > 255 ? 255 : y_val);
        hist[y_val]++;
    }

    // Step 3: Compute CDF of Y channel
    unsigned int cdf[256] = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + hist[i];
    }

    // Find minimum non-zero CDF value
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Step 4: Create equalization mapping for Y channel
    unsigned int hist_eq[256] = {0};
    unsigned int N = img->width * img->height;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            hist_eq[i] = (unsigned int)round(((float)(cdf[i] - cdf_min) / (N - cdf_min)) * 255);
        } else {
            hist_eq[i] = 0;
        }
    }

    // Step 5: Apply equalization to Y channel and convert back to RGB
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            // Apply equalization to Y component
            int y_val = (int)round(yuv_img[i][j].Y);
            y_val = y_val < 0 ? 0 : (y_val > 255 ? 255 : y_val);
            yuv_img[i][j].Y = (float)hist_eq[y_val];

            // Convert back to RGB
            img->data[i][j] = yuv_to_rgb(yuv_img[i][j]);
        }
        free(yuv_img[i]);
    }
    free(yuv_img);
    free(Y_values);
}

