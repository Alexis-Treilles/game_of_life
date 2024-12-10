#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#define iterations 100
/*

Ouverture, écriture, exécution des regles du jeux sur 2 images.
*/
void open_img(const char *filename, unsigned char **image, int *width, int *height) {
    FILE *img = fopen(filename, "r");
    if (!img) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Lire le format et les dimensions
    char format[3];
    fgets(format, sizeof(format), img);
    fscanf(img, "%d %d", width, height);

    int new_width = *width + 2;  // Nouvelle largeur avec la bordure
    int new_height = *height + 2; // Nouvelle hauteur avec la bordure

    // Allouer la mémoire pour l'image étendue avec la bordure
    *image = (unsigned char *)calloc(new_width * new_height, sizeof(unsigned char));
    if (!*image) {
        perror("Erreur d'allocation mémoire");
        fclose(img);
        exit(EXIT_FAILURE);
    }

    // Lire les pixels et les insérer au centre de l'image étendue
    for (int y = 1; y <= *height; y++) {
        for (int x = 1; x <= *width; x++) {
            int pixel;
            fscanf(img, "%d", &pixel);
            (*image)[y * new_width + x] = (unsigned char)pixel;
        }
    }

    fclose(img);
}


// Écrire les pixels dans un fichier
void write_img(const char *filename, unsigned char *image, int width, int height) {
    FILE *img = fopen(filename, "w");
    // Écrire l'en-tête
    fprintf(img, "P1\n%d %d\n", width+2, height+2);

    int dim = (width+2) * (height+2);
    //printf("%d \n", dim);
    // Écrire les pixels
    for (int i = 0; i < dim; i++) {
        fprintf(img, "%d", image[i]);
        fprintf(img, " ");
    }
    fclose(img);
}

// Appliquer les règles du Jeu de la Vie
void apply_game_of_life(unsigned char *current_image, unsigned char *new_image, int width, int height) {
    int padded_width = width + 2; // Largeur totale avec les bordures

    for (int index = 0; index < width * height; index++) {
        // Convertir l'index 1D en coordonnées 2D dans la zone centrale
        int x = (index % width) + 1;  // Décalage pour correspondre à la zone centrale
        int y = (index / width) + 1;  // Décalage pour correspondre à la zone centrale

        int live_neighbors = 0;

        // Calcul des voisins vivants (en utilisant la largeur totale avec bordures)
        live_neighbors += current_image[(y - 1) * padded_width + (x - 1)];
        live_neighbors += current_image[(y - 1) * padded_width + x];
        live_neighbors += current_image[(y - 1) * padded_width + (x + 1)];
        live_neighbors += current_image[y * padded_width + (x - 1)];
        live_neighbors += current_image[y * padded_width + (x + 1)];
        live_neighbors += current_image[(y + 1) * padded_width + (x - 1)];
        live_neighbors += current_image[(y + 1) * padded_width + x];
        live_neighbors += current_image[(y + 1) * padded_width + (x + 1)];

        // Application des règles du jeu de la vie
        if (current_image[y * padded_width + x] == 1) { // Cellule vivante
            if (live_neighbors == 2 || live_neighbors == 3) {
                new_image[y * padded_width + x] = 1; // Reste vivante
            } else {
                new_image[y * padded_width + x] = 0; // Meurt
            }
        } else { // Cellule morte
            if (live_neighbors == 3) {
                new_image[y * padded_width + x] = 1; // Devient vivante
            } else {
                new_image[y * padded_width + x] = 0; // Reste morte
            }
        }
    }
}



// Main
int main() {
    int width, height;

    // Créer le répertoire pour stocker les images
    system("mkdir -p images");

    // Allouer la mémoire pour les images
    unsigned char *current_image=NULL;

    // Charger l'image initiale
    open_img("empty_image_1920x1080.pbm", &current_image, &width, &height);

    printf("Image loaded: width = %d, height = %d\n", width, height);

    // Sauvegarder la première image
    char filename[256];
    snprintf(filename, sizeof(filename), "images/frame_0000.pbm");
    write_img(filename, current_image, width, height);
    
    unsigned char *new_image = (unsigned char *)calloc((width+2) * (height+2), sizeof(unsigned char));
    // Mesurer le temps total
    clock_t start = clock();

    // Appliquer les règles pour 1000 itérations
    for (int i = 1; i <= iterations; i++) {
        apply_game_of_life(current_image, new_image, width, height);

        // Sauvegarder l'image générée
        snprintf(filename, sizeof(filename), "images/frame_%04d.pbm", i);
        write_img(filename, new_image, width, height);

        // Échanger les buffers
        unsigned char *temp = current_image;
        current_image = new_image;
        new_image = temp;
    }

    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Temps total pour %d itérations : %.2f secondes\n", iterations, time_taken);

    // Libérer la mémoire
    free(current_image);
    free(new_image);

    return 0;
}