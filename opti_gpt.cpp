#include <immintrin.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
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
        fprintf(img, "%d", image[i]);
        if ((i + 1) % width == 0)
            fprintf(img, "\n");
        else
            fprintf(img, " ");
    }
    fclose(img);
}

// Appliquer les règles du Jeu de la Vie
void apply_game_of_life_avx(unsigned char *current_image, unsigned char *new_image, int width, int height) {
    #pragma omp parallel for
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x += 32) { // AVX traite 32 pixels (256 bits)
            __m256i live_neighbors = _mm256_setzero_si256();

            // Charger les 8 voisins
            __m256i top = _mm256_loadu_si256((__m256i *)&current_image[(y - 1) * width + x]);
            __m256i bottom = _mm256_loadu_si256((__m256i *)&current_image[(y + 1) * width + x]);
            __m256i left = _mm256_loadu_si256((__m256i *)&current_image[y * width + x - 1]);
            __m256i right = _mm256_loadu_si256((__m256i *)&current_image[y * width + x + 1]);

            live_neighbors = _mm256_add_epi8(live_neighbors, top);
            live_neighbors = _mm256_add_epi8(live_neighbors, bottom);
            live_neighbors = _mm256_add_epi8(live_neighbors, left);
            live_neighbors = _mm256_add_epi8(live_neighbors, right);

            // Charger la cellule actuelle
            __m256i current = _mm256_loadu_si256((__m256i *)&current_image[y * width + x]);

            // Règles : Reste vivante (2 ou 3 voisins), Devient vivante (exactement 3 voisins)
            __m256i is_two = _mm256_cmpeq_epi8(live_neighbors, _mm256_set1_epi8(2));
            __m256i is_three = _mm256_cmpeq_epi8(live_neighbors, _mm256_set1_epi8(3));
            __m256i stay_alive = _mm256_and_si256(current, is_two);
            __m256i become_alive = is_three;

            __m256i next_state = _mm256_or_si256(stay_alive, become_alive);

            // Sauvegarder le nouvel état
            _mm256_storeu_si256((__m256i *)&new_image[y * width + x], next_state);
        }
    }
}

// Main
int main() {
    int width, height;
    int iterations = 1000;

    system("mkdir -p images");

    get_image_dimensions("image_init.ppm", &width, &height);

    unsigned char *current_image = (unsigned char *)aligned_alloc(32, width * height * sizeof(unsigned char));
    unsigned char *new_image = (unsigned char *)aligned_alloc(32, width * height * sizeof(unsigned char));

    open_img("image_init.ppm", current_image, width, height);

    printf("Image loaded: width = %d, height = %d\n", width, height);

    clock_t start = clock();

    for (int i = 0; i < iterations; i++) {
        apply_game_of_life_avx(current_image, new_image, width, height);

        char filename[256];
        snprintf(filename, sizeof(filename), "images/frame_%04d.ppm", i);
        write_img(filename, new_image, width, height);

        unsigned char *temp = current_image;
        current_image = new_image;
        new_image = temp;
    }

    clock_t end = clock();
    printf("Temps total : %.2f secondes\n", (double)(end - start) / CLOCKS_PER_SEC);

    free(current_image);
    free(new_image);

    return 0;
}
