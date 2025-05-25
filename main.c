#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

int main() {
    t_bmp8 *img = NULL;
    char inputPath[100];
    char outputPath[100];
    int choice;
    int value;

    do {
        printf("\n=== BMP Image Processing Menu ===\n");
        printf("1. Load image\n");
        printf("2. Display image info\n");
        printf("3. Apply negative\n");
        printf("4. Apply brightness\n");
        printf("5. Apply threshold\n");
        printf("6. Apply box blur\n");
        printf("7. Apply histogram equalization\n");
        printf("8. Save image\n");
        printf("9. Exit\n");
        printf("Your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter image path: ");
                scanf("%s", inputPath);
                if (img) bmp8_free(img);
                img = bmp8_loadImage(inputPath);
                if (!img) printf("Failed to load image.\n");
                else printf("Image loaded successfully.\n");
                break;

            case 2:
                if (img) bmp8_printInfo(img);
                else printf("No image loaded.\n");
                break;

            case 3:
                if (img) bmp8_negative(img);
                else printf("No image loaded.\n");
                break;

            case 4:
                if (img) {
                    printf("Enter brightness value (+/-): ");
                    scanf("%d", &value);
                    bmp8_brightness(img, value);
                } else printf("No image loaded.\n");
                break;

            case 5:
                if (img) {
                    printf("Enter threshold (0–255): ");
                    scanf("%d", &value);
                    bmp8_threshold(img, value);
                } else printf("No image loaded.\n");
                break;

            case 6:
                if (img) bmp8_boxBlur(img);
                else printf("No image loaded.\n");
                break;

            case 7:
                if (img) bmp8_histogramEqualization(img);
                else printf("No image loaded.\n");
                break;

            case 8:
                if (img) {
                    printf("Enter output path: ");
                    scanf("%s", outputPath);
                    bmp8_saveImage(outputPath, img);
                    printf("Image saved to %s\n", outputPath);
                } else printf("No image loaded.\n");
                break;

            case 9:
                printf("Goodbye!\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 9);

    if (img) bmp8_free(img);
    return 0;
}