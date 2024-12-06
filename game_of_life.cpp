#include <stdio.h>
#include <stdlib.h>

void open_img(const char *filename, char *image, int *width, int *height) {
    FILE* img = fopen(filename, "r");
    if (img == NULL) {
        exit(EXIT_FAILURE);
    }

    // Lire le format (P1)
    fgetc(img); // Ignorer 'P'
    fgetc(img); // Ignorer '1'

    // Lire les dimensions
    fscanf(img, "%d %d", width, height);

    if (image == NULL) {
        fclose(img);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < (*width) * (*height); i++) {
        fscanf(img, "%hhd", &image[i]);
        printf("%hhd ", image[i]);
    }

    fclose(img);
}

void write_img(const char *filename, char *image, int width, int height) {
    FILE* img=fopen(filename, "w");
    fprintf(img, "P1 %d %d ", width, height);
    for (int i = 0; i < height*width; i++) {
            printf("%d ", image[i]);
            fprintf(img, "%hhd", image[i]);
            fputc(' ', img);
    }
    fclose(img);
}

int main(int argc, char** argv) {
    char *image;
    image = (char*)calloc((*width) * (*height), sizeof(char));
    int width, height;

    open_img("image.ppm", image, &width, &height);
    printf("Image loaded: width = %d, height = %d\n", width, height);
    printf("\n");
    write_img("output.ppm", image, width, height);
    return 0;
}
