#include <immintrin.h> // Pour AVX
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Lire uniquement les dimensions de l'image
void get_image_dimensions(const char *filename, int *width, int *height) {
    FILE *img = fopen(filename, "r");
    char format[3];
    fgets(format, sizeof(format), img);
    fscanf(img, "%d %d", width, height);
    fclose(img);
}

// Lire les pixels de l'image
void open_img(const char *filename, unsigned char *image, int width, int height) {
    FILE *img = fopen(filename, "r");
    char format[3];
    fgets(format, sizeof(format), img);
    fscanf(img, "%d %d", &width, &height);
    for (int i = 0; i < width * height; i++) {
        int pixel;
        fscanf(img, "%d", &pixel);
        image[i] = (unsigned char)pixel;
    }
    fclose(img);
}

// Écrire les pixels dans un fichier
void write_img(const char *filename, unsigned char *image, int width, int height) {
    FILE *img = fopen(filename, "w");
    fprintf(img, "P1\n%d %d\n", width, height);
    for (int i = 0; i < width * height; i++) {
        fprintf(img, "%d ", image[i]);
    }
    fclose(img);
}

// Compter les voisins vivants avec AVX
void count_live_neighbors_avx(unsigned char *current_image, int *neighbor_counts, int width, int height) {
    #pragma omp parallel for
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x += 8) { // Traite 8 pixels à la fois
            __m256i neighbors = _mm256_setzero_si256();

            // Charger les 8 voisins (en déroulant les décalages)
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue; // Ignorer le centre
                    __m256i neighbor = _mm256_loadu_si256((__m256i *)&current_image[(y + dy) * width + (x + dx)]);
                    neighbors = _mm256_add_epi32(neighbors, neighbor);
                }
            }

            // Sauvegarder les résultats
            _mm256_storeu_si256((__m256i *)&neighbor_counts[y * width + x], neighbors);
        }
    }
}

// Appliquer les règles du Jeu de la Vie avec AVX
void apply_game_of_life_avx(unsigned char *current_image, unsigned char *new_image, int *neighbor_counts, int width, int height) {
    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x += 8) { // Traite 8 pixels à la fois
            __m256i neighbors = _mm256_loadu_si256((__m256i *)&neighbor_counts[y * width + x]);
            __m256i current = _mm256_loadu_si256((__m256i *)&current_image[y * width + x]);

            // Cellules vivantes : (neighbors == 2 || neighbors == 3)
            __m256i eq2 = _mm256_cmpeq_epi32(neighbors, _mm256_set1_epi32(2));
            __m256i eq3 = _mm256_cmpeq_epi32(neighbors, _mm256_set1_epi32(3));
            __m256i stay_alive = _mm256_or_si256(eq2, eq3);

            // Cellules mortes : (neighbors == 3)
            __m256i become_alive = _mm256_cmpeq_epi32(neighbors, _mm256_set1_epi32(3));

            // Calcul final : vivante si vivante && stay_alive ou morte && become_alive
            __m256i result = _mm256_or_si256(
                _mm256_and_si256(current, stay_alive),
                _mm256_andnot_si256(current, become_alive)
            );

            // Sauvegarder les résultats
            _mm256_storeu_si256((__m256i *)&new_image[y * width + x], result);
        }
    }
}

// Main
int main() {
    int width, height;
    int iterations = 100;

    system("mkdir -p images"); // Créer le répertoire des images

    // Charger les dimensions et initialiser les images
    get_image_dimensions("image_init.ppm", &width, &height);
    unsigned char *current_image = (unsigned char *)aligned_alloc(32, width * height * sizeof(unsigned char));
    unsigned char *new_image = (unsigned char *)aligned_alloc(32, width * height * sizeof(unsigned char));
    int *neighbor_counts = (int *)aligned_alloc(32, width * height * sizeof(int));
    open_img("image_init.ppm", current_image, width, height);

    // Mesurer le temps
    clock_t start = clock();

    // Générer les itérations
    for (int i = 0; i < iterations; i++) {
        count_live_neighbors_avx(current_image, neighbor_counts, width, height);
        apply_game_of_life_avx(current_image, new_image, neighbor_counts, width, height);

        // Sauvegarder l'image
        char filename[256];
        snprintf(filename, sizeof(filename), "images/frame_%04d.ppm", i);
        write_img(filename, new_image, width, height);

        // Échanger les buffers
        unsigned char *temp = current_image;
        current_image = new_image;
        new_image = temp;
    }

    clock_t end = clock();
    printf("Temps total : %.2f secondes\n", (double)(end - start) / CLOCKS_PER_SEC);

    // Libérer la mémoire
    free(current_image);
    free(new_image);
    free(neighbor_counts);

    return 0;
}
