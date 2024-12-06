#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Lire uniquement les dimensions de l'image
void get_image_dimensions(const char *filename, int *width, int *height) {
    FILE* img = fopen(filename, "r");

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

// Appliquer les règles du Jeu de la Vie
void apply_game_of_life(char *current_image, char *new_image, int width, int height) {
    int cells_created = 0; // Compteur de cellules créées (mortes → vivantes)
    int cells_killed = 0;  // Compteur de cellules tuées (vivantes → mortes)

    // Parcourir chaque cellule de la matrice
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Compter les voisins vivants
            int live_neighbors = 0;

            // Parcourir les 8 voisins
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    // Ignorer la cellule elle-même
                    if (dx == 0 && dy == 0) continue;

                    // Coordonnées du voisin
                    int nx = x + dx;
                    int ny = y + dy;

                    // Vérifier que le voisin est dans les limites
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        live_neighbors += current_image[ny * width + nx];
                    }
                }
            }

            // Appliquer les règles du Jeu de la Vie
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

    // Afficher le nombre de cellules créées et tuées
    printf("Cells created: %d\n", cells_created);
    printf("Cells killed: %d\n", cells_killed);
}

// Sauvegarder une frame
void save_frame(const char *directory, char *image, int width, int height, int frame) {
    // Créer un nom de fichier unique pour chaque image
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/frame_%03d.ppm", directory, frame);

    // Sauvegarder l'image dans le fichier
    write_img(filename, image, width, height);
}

int main(int argc, char** argv) {
    int width, height;
    int iterations = 100;
    int fps = 5;

    // Créer le répertoire pour les images (si nécessaire)
    system("mkdir -p images");

    // Obtenir les dimensions de l'image
    get_image_dimensions("image_init.ppm", &width, &height);

    // Allouer la mémoire pour les images
    char *current_image = (char*)calloc(width * height, sizeof(char));
    char *new_image = (char*)calloc(width * height, sizeof(char));
    if (current_image == NULL || new_image == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    // Charger l'image initiale
    open_img("image_init.ppm", current_image, width, height);

    printf("Image loaded: width = %d, height = %d\n", width, height);

    // Sauvegarder la première image
    save_frame("images", current_image, width, height, 0);

    // Générer les itérations et sauvegarder les images
    for (int i = 1; i <= iterations; i++) {
        apply_game_of_life(current_image, new_image, width, height);

        // Sauvegarder l'image générée
        save_frame("images", new_image, width, height, i);

        // Échanger les images pour la prochaine itération
        char *temp = current_image;
        current_image = new_image;
        new_image = temp;
    }

    // Libérer la mémoire
    free(current_image);
    free(new_image);

    // Commande pour créer la vidéo avec FFmpeg
    printf("Run the following command to generate the video:\n");
    printf("ffmpeg -y -framerate %d -i images/frame_%%03d.ppm -c:v libx264 -pix_fmt yuv420p output.mp4\n", fps);

    return 0;
}
