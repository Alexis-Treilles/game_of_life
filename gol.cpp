#include <stdio.h>
#include <stdlib.h>

// Lire uniquement les dimensions de l'image
void get_image_dimensions(const char *filename, int *width, int *height) {
    FILE* img = fopen(filename, "r");
    if (img == NULL) {
        perror("Erreur à l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Lire le format (P1)
    char format[3];
    if (fgets(format, sizeof(format), img) == NULL || format[0] != 'P' || format[1] != '1') {
        fprintf(stderr, "Format non valide\n");
        fclose(img);
        exit(EXIT_FAILURE);
    }

    // Lire les dimensions
    if (fscanf(img, "%d %d", width, height) != 2) {
        fprintf(stderr, "Erreur lors de la lecture des dimensions\n");
        fclose(img);
        exit(EXIT_FAILURE);
    }

    fclose(img);
}

// Lire les pixels de l'image
void open_img(const char *filename, char *image, int width, int height) {
    FILE* img = fopen(filename, "r");
    if (img == NULL) {
        perror("Erreur à l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Ignorer le format et les dimensions
    char format[3];
    fgets(format, sizeof(format), img);
    fscanf(img, "%d %d", &width, &height);

    // Lire les pixels
    for (int i = 0; i < width * height; i++) {
        fscanf(img, "%hhd", &image[i]);
    }

    fclose(img);
}

// Écrire les pixels dans un fichier
void write_img(const char *filename, char *image, int width, int height) {
    FILE* img = fopen(filename, "w");
    if (img == NULL) {
        perror("Erreur à l'ouverture du fichier en écriture");
        exit(EXIT_FAILURE);
    }

    // Écrire l'en-tête
    fprintf(img, "P1\n%d %d\n", width, height);

    // Écrire les pixels
    for (int i = 0; i < width * height; i++) {
        fprintf(img, "%d", image[i]);
        if ((i + 1) % width == 0) {
            fprintf(img, "\n"); // Nouvelle ligne après chaque rangée
        } else {
            fprintf(img, " "); // Séparer les pixels par un espace
        }
    }

    fclose(img);
}

int main(int argc, char** argv) {
    int width, height;

    // Étape 1 : Obtenir les dimensions de l'image
    get_image_dimensions("image.ppm", &width, &height);

    // Étape 2 : Allouer la mémoire pour l'image
    char *image = (char*)calloc(width * height, sizeof(char));
    if (image == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Étape 3 : Charger les pixels de l'image
    open_img("image.ppm", image, width, height);

    printf("Image loaded: width = %d, height = %d\n", width, height);

    // Étape 4 : Écrire l'image dans un nouveau fichier
    write_img("output.ppm", image, width, height);

    // Libérer la mémoire
    free(image);

    return 0;
}
