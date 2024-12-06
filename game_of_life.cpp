#include <stdio.h>
#include <stdlib.h>
/*

Ouverture, écriture, exécution des regles du jeux sur 2 images.

*/
// Lire uniquement les dimensions de l'image
void get_image_dimensions(const char *filename, int *width, int *height) {
    FILE *img = fopen(filename, "r");
    if (!img) {
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
void open_img(const char *filename, unsigned char *image, int width, int height) {
    FILE *img = fopen(filename, "r");
    if (!img) {
        perror("Erreur à l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Ignorer le format et les dimensions
    char format[3];
    fgets(format, sizeof(format), img);
    fscanf(img, "%d %d", &width, &height);

    // Lire les pixels
    for (int i = 0; i < width * height; i++) {
        int pixel;
        if (fscanf(img, "%d", &pixel) != 1 || (pixel != 0 && pixel != 1)) {
            fprintf(stderr, "Erreur : pixel invalide (%d) à l'indice %d\n", pixel, i);
            fclose(img);
            exit(EXIT_FAILURE);
        }
        image[i] = (unsigned char)pixel;
    }

    fclose(img);
}

// Écrire les pixels dans un fichier
void write_img(const char *filename, unsigned char *image, int width, int height) {
    FILE *img = fopen(filename, "w");
    if (!img) {
        perror("Erreur à l'ouverture du fichier pour l'écriture");
        exit(EXIT_FAILURE);
    }

    // Écrire l'en-tête
    fprintf(img, "P1\n%d %d\n", width, height);

    // Écrire les pixels
    for (int i = 0; i < width * height; i++) {
        fprintf(img, "%d", image[i]);
        if ((i + 1) % width == 0) {
            fprintf(img, "\n");
        } else {
            fprintf(img, " ");
        }
    }

    fclose(img);
}

// Compter les voisins vivants
int count_live_neighbors(unsigned char *current_image, int width, int height, int x, int y) {
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

    int live_neighbors = 0;

    for (int i = 0; i < 8; i++) {
        int nx = x + offsets[i][0];
        int ny = y + offsets[i][1];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            live_neighbors += current_image[ny * width + nx];
        }
    }

    return live_neighbors;
}

// Appliquer les règles du Jeu de la Vie
void apply_game_of_life(unsigned char *current_image, unsigned char *new_image, int width, int height) {
    int cells_created = 0, cells_killed = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int live_neighbors = count_live_neighbors(current_image, width, height, x, y);
            int index = y * width + x;

            if (current_image[index] == 1) { // Cellule vivante
                if (live_neighbors == 2 || live_neighbors == 3) {
                    new_image[index] = 1; // Reste vivante
                } else {
                    new_image[index] = 0; // Meurt
                    cells_killed++;
                }
            } else { // Cellule morte
                if (live_neighbors == 3) {
                    new_image[index] = 1; // Devient vivante
                    cells_created++;
                } else {
                    new_image[index] = 0; // Reste morte
                }
            }
        }
    }

    printf("Cells created: %d\n", cells_created);
    printf("Cells killed: %d\n", cells_killed);
}

// Main
int main() {
    int width, height;

    // Obtenir les dimensions de l'image
    get_image_dimensions("image_init.ppm", &width, &height);

    // Allouer la mémoire pour les images
    unsigned char *current_image = (unsigned char *)calloc(width * height, sizeof(unsigned char));
    unsigned char *new_image = (unsigned char *)calloc(width * height, sizeof(unsigned char));
    if (!current_image || !new_image) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Charger l'image initiale
    open_img("image_init.ppm", current_image, width, height);

    printf("Image loaded: width = %d, height = %d\n", width, height);

    // Appliquer les règles du Jeu de la Vie
    apply_game_of_life(current_image, new_image, width, height);

    // Écrire l'image résultante
    write_img("output.ppm", new_image, width, height);
    apply_game_of_life(new_image, current_image, width, height);
    write_img("output2.ppm", current_image, width, height);
    // Libérer la mémoire
    free(current_image);
    free(new_image);

    return 0;
}


