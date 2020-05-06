#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

struct PGM {
  int width;
  int height;
  int M;
} typedef PGM;

PGM* read_header(const char* filename);
FILE* init(const char* filename);
int read_enxt(FILE* ptr);
int read_kth(FILE* ptr, int k);
void ign_k(FILE* ptr, int k);
void print_res(int* data, int m);
void save_res(int* results, int m, const char* filename);