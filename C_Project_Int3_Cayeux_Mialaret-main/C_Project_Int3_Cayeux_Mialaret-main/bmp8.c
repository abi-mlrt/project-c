//
// Created by abiga on 27/04/2025, modified by Raphaël on 20/05/2025
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"
#include <math.h>

//1.2.1 The bmp8_loadImage function
t_bmp8 * bmp8_loadImage(const char * filename){
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL){
        printf("Can't open the file %s", filename);
        return NULL;
    }

    t_bmp8 * img = (t_bmp8 *) malloc(sizeof(t_bmp8));
    if (img==NULL) {
        printf("Can't allocate memory for the image");
        fclose(fp);
        return NULL;
    }
    //read header
    if (fread(img->header, sizeof(unsigned char), 54, fp) != 54) {
        printf("Couldn't read the BMP header");
        free(img);
        fclose(fp);
        return NULL;
    }
    if (img->header[0] != 'B' || img->header[1] != 'M') {
        printf("File is not a BMP file");
        free(img);
        fclose(fp);
        return NULL;
    }
    //img info
    img->width = *(int*)&img->header[18];
    img->height = *(int*)&img->header[22];
    img->colorDepth = *(short*)&img->header[28];
    img->dataSize = *(unsigned int*)&img->header[34];
    //8 bits img?
    if (img->colorDepth != 8) {
        printf("Only 8-bit grayscale BMP images are supported \n");
        free(img);
        fclose(fp);
        return NULL;
    }

    if (fread(img->colorTable, sizeof(unsigned char), 1024, fp)!=1024){
        printf("Couldn't read the color table\n");
        free(img);
        fclose(fp);
        return NULL;
    }

unsigned int rowSize = ( (img->width + 3) / 4 ) * 4;
    img->dataSize = rowSize * img->height;

    img->data = (unsigned char *) malloc(img->dataSize);
    if (img->data == NULL) {
        printf("Impossible to allocate memory");
        free(img);
        fclose(fp);
        return img;
    }

    if (fread(img->data, sizeof(unsigned char), img->dataSize, fp) != img->dataSize) {
        printf("Couldn't read the data of the image");
        free(img->data);
        free(img);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return img;
}
//1.2.2 The bmp8_saveImage function
void bmp8_saveImage(const char *filename, const t_bmp8 *img){
    FILE *fp = fopen(filename, "wb");
    if(fp == NULL){
        printf("Couldn't open %s for writing\n", filename);
        return;
    }

    if (fwrite(img->header, 1, 54, fp) != 54)
        printf("Failed to write header.\n");

    if (fwrite(img->colorTable, 1, 1024, fp) != 1024)
        printf("Failed to write color table.\n");

    if (fwrite(img->data, 1, img->dataSize, fp) != img->dataSize)
        printf("Failed to write pixel data.\n");
    else
        printf("Image saved successfully to %s\n", filename);

    fclose(fp);
}

//1.2.3 The bmp8_free function
void bmp8_free(t_bmp8 *img){
    if(img!=NULL){
        if(img->data!=NULL){
            free(img->data);
        }
        free(img);
    }
}
//1.2.4 The bmp8_printInfo function
void bmp8_printInfo(const t_bmp8 *img){
    if(img == NULL){
        printf("No available info");
        return;
    }

    printf("Width: %u px\n", img->width);
    printf("Height: %u px\n", img->height);
    printf("Color depth: %u bits\n", img->colorDepth);
    printf("Image data size: %u bytes\n", img->dataSize);
}


//1.3 Image Processing Functions
void bmp8_negative(t_bmp8 * img){
    if (!img || !img->data) return;  // Safety check
    unsigned int totalPixels = img->width * img->height;
    for (unsigned int i = 0; i < totalPixels; i++) {
        img->data[i]= 255-img->data[i];
    }
}
void bmp8_brightness(t_bmp8 * img, int value){
    if (!img || !img->data) return;  // Safety check
    unsigned int totalPixels = img->width * img->height;
    for (unsigned int i = 0; i < totalPixels; i++) {
        int new_value = img->data[i] + value;
        if (new_value > 255) new_value = 255;
        if (new_value < 0) new_value = 0;
        img->data[i] = (unsigned char)new_value;

    }
}
void bmp8_threshold(t_bmp8 * img, int threshold){
        if (!img || !img->data) return;  // Safety check
        const unsigned int totalPixels = img->width * img->height;
        for (unsigned int i = 0; i < totalPixels; i++){
            if (img->data[i] >= threshold){img->data[i] = 255;}
            if (img->data[i] < threshold){img->data[i] = 0;}
  }
}

//1.4 Image filtering
//function to alloc a 2D array for the kernel
float** allocateKernel(int size) {
        float** kernel =(float**)malloc(size * sizeof(float*));
        for (int i = 0;i<size;i++) {
            kernel[i]=(float*)malloc(size*sizeof(float));
        }
        return kernel;
    }
//function to free the kernel
void freekernel(float** kernel,int size) {
    for ( int i = 0;i < size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}
void bmp8_applyFilter(t_bmp8* img, float** kernel, int kernelSize) {
    // Validate inputs
    if (!img || !img->data || !kernel || kernelSize % 2 == 0) return;

    // Create temp buffer
    unsigned char* tempData = (unsigned char*)malloc(img->dataSize);
    if (!tempData) return;
    memcpy(tempData, img->data, img->dataSize);

    const int n = kernelSize / 2;  // Kernel radius

    // Apply convolution
    for (unsigned int y = n; y < img->height - n; y++) {
        for (unsigned int x = n; x < img->width - n; x++) {
            float sum = 0.0f;

            // Convolve with kernel
            for (int ky = -n; ky <= n; ky++) {
                for (int kx = -n; kx <= n; kx++) {
                    unsigned int pixelPos = (y + ky) * img->width + (x + kx);
                    sum += tempData[pixelPos] * kernel[ky + n][kx + n];
                }
            }

            // Clamp and store
            if (sum > 255) sum = 255;
            else if (sum < 0) sum = 0;
            img->data[y * img->width + x] = (unsigned char)sum;;
        }
    }

    free(tempData);
}


void bmp8_boxBlur(t_bmp8* img) {
    if (!img || !img->data) return;
    float** kernel = allocateKernel(3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }

    bmp8_applyFilter(img, kernel, 3);
    freekernel(kernel, 3);
}

void bmp8_gaussianBlur(t_bmp8* img) {
    float** kernel = allocateKernel(3);
    float gaussianKernel[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = gaussianKernel[i][j];
        }
    }

    bmp8_applyFilter(img, kernel, 3);
    freekernel(kernel, 3);
}

void bmp8_outline(t_bmp8* img) {
    float** kernel = allocateKernel(3);
    float outlineKernel[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = outlineKernel[i][j];
        }
    }

    bmp8_applyFilter(img, kernel, 3);
    freekernel(kernel, 3);
}

void bmp8_emboss(t_bmp8* img) {
    float** kernel = allocateKernel(3);
    float embossKernel[3][3] = {
        {-2, -1,  0},
        {-1,  1,  1},
        { 0,  1,  2}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = embossKernel[i][j];
        }
    }

    bmp8_applyFilter(img, kernel, 3);
    freekernel(kernel, 3);
}

void bmp8_sharpen(t_bmp8* img) {
    float** kernel = allocateKernel(3);
    float sharpenKernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = sharpenKernel[i][j];
        }
    }

    bmp8_applyFilter(img, kernel, 3);
    freekernel(kernel, 3);
}

void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    // Apply the equalization mapping to each pixel
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)hist_eq[img->data[i]];
    }
}
unsigned * bmp8_computeHistogram(t_bmp8 *img) {
    if (img == NULL || img->data == NULL) {
        return NULL;
    }
    // Allocate histogram array (256 bins for 8-bit image)
    unsigned int *hist = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!hist) return NULL;

    // Count pixel values
    for (unsigned int i = 0; i < img->dataSize; i++) {
        hist[img->data[i]]++;
    }

    return hist;
}

unsigned *bmp8_computeCDF(unsigned int *hist_eq) {
    // Allocate CDF array
    unsigned int *cdf = (unsigned int *)malloc(256 * sizeof(unsigned int));
    if (!cdf) return NULL;
    // Compute cumulative histogram
    cdf[0] = hist_eq[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + hist_eq[i];
    }
    return cdf;}

// Convenience function to perform full histogram equalization
void bmp8_histogramEqualization(t_bmp8 *img) {
    // Step 1: Compute histogram
    unsigned int *hist = bmp8_computeHistogram(img);
    if (!hist) return;

    // Step 2: Compute CDF
    unsigned int *cdf = bmp8_computeCDF(hist);
    if (!cdf) {
        free(hist);
        return;
    }

    // Find minimum non-zero value in CDF
    unsigned int cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Step 3: Normalize CDF to create equalization mapping
    unsigned int *hist_eq = (unsigned int *)malloc(256 * sizeof(unsigned int));
    if (!hist_eq) {
        free(hist);
        free(cdf);
        return;
    }

    unsigned int N = img->width * img->height;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            hist_eq[i] = (unsigned int)round(((float)(cdf[i] - cdf_min) / (N - cdf_min)) * 255);
        } else {
            hist_eq[i] = 0;
        }
    }

    // Step 4: Apply equalization
    bmp8_equalize(img, hist_eq);

    // Clean up
    free(hist);
    free(cdf);
    free(hist_eq);
}