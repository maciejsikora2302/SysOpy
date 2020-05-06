#include "util.h"

PGM* read_header(const char* filename) {
    PGM*  header = malloc(sizeof(PGM));
    FILE* ptr = fopen(filename, "r");

    char magic_number[16];
    // Read magic number and ignore following comment.
    fscanf(ptr, "%s\n#%*[^\n]\n", magic_number);
    fscanf(ptr, "%d %d %d", &header->width, &header->height, &header->M);

    fclose(ptr);
    return header;
}

FILE* init(const char* filename) {
    FILE* ptr = fopen(filename, "r");

    fscanf(ptr, "%*s\n#%*[^\n]\n");
    fscanf(ptr, "%*d %*d %*d");

    return ptr;
}

int read_enxt(FILE* ptr) {
    int v;
    fscanf(ptr, "%d", &v);

    return v;
}

int read_kth(FILE* ptr, int k) {
    int v;
    while (k-- > 0) {
        fscanf(ptr, "%d", &v);
    }

    return v;
}

void ign_k(FILE* ptr, int k) {
    int x;
    while (k-- > 0) {
        fscanf(ptr, "%d", &x);
    }
}

void print_res(int* data, int m) {
    printf("===================================Matrix histogram===================================\n");
    int sq = (int) sqrt((double) m);
    for (int i = 0; i < m; i++) {
        printf("%04d ", data[i]);
        if (i % sq == (sq - 1)) {
            printf("\n");
        }
    }       
    printf("=======================================================================================\n");
}

void save_res(int* results, int m, const char* filename) {
    FILE* ptr = fopen(filename, "w");
    for (int i = 0; i < m; i++) {
        fprintf(ptr, "%d %d\n", i, results[i]);
    }
}