#include "util.h"

int m;
char* filename;
PGM* header;
int** thread_res;

struct SignThreadArg {
    int from;
    int until;
    int k;
} typedef SignThreadArg;

struct ThreadArg {
    int k;
} typedef ThreadArg;


long int get_time_micro_s(struct timespec start, struct timespec end) {
    long int elapsedTime;

    elapsedTime = (end.tv_sec - start.tv_sec) * 1e6;
    elapsedTime += (end.tv_nsec - start.tv_nsec) * 1e-3;

    return elapsedTime;
}

void* thread_sign(void* sign_t_arg) {
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    
    SignThreadArg* arg = (SignThreadArg*) sign_t_arg;
    thread_res[arg -> k]  = calloc(header -> M + 1, sizeof(int)); 

    FILE* file_ptr = init(filename);
    int value;
    for (int i = 0; i < (header -> width) * (header -> height); i++) {
        value = read_enxt(file_ptr);
        if (value >= arg -> from && value < arg -> until) {
            thread_res[arg -> k][value] += 1;
        }
    }

    free(arg);
    fclose(file_ptr);
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);


    return (void*) get_time_micro_s(start, end);
}

void* thread_block(void* thr_arg) {
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    ThreadArg* arg = (ThreadArg*) thr_arg;
    thread_res[arg -> k]  = calloc(header -> M + 1, sizeof(int)); 
    FILE* file_ptr = init(filename);

    int readK = (header -> width) / m;
    int ignore = (header -> width) - readK; 
    // Just in case we get odd number.
    if (arg -> k == m - 1) {
        readK += readK % m; 
    }

    int value;
    for (int j = 0; j < header -> height; j++) {
        if (j == 0) {
           ign_k(file_ptr, arg -> k * readK); 
        } else {
           ign_k(file_ptr, ignore); 
        }

        for (int i = 0; i < readK; i++) {
            value = read_enxt(file_ptr);
            thread_res[arg -> k][value] += 1;
        }
    }
    
    free(arg);
    fclose(file_ptr);
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);


    return (void*) get_time_micro_s(start, end);
}

void* thread_interleaved(void* thr_arg) {
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    ThreadArg* arg = (ThreadArg*) thr_arg;
    thread_res[arg -> k]  = calloc(header -> M + 1, sizeof(int)); 
    FILE* file_ptr = init(filename);

    int value;
    int leapSize = m - 1;
    int max = header -> width * header -> height;

    ign_k(file_ptr, arg -> k);
    value = read_enxt(file_ptr);
    thread_res[arg -> k][value] += 1;
    for (int i = arg -> k + leapSize + 1; i < max; i += leapSize + 1) {
        ign_k(file_ptr, leapSize);
        value = read_enxt(file_ptr);
        thread_res[arg -> k][value] += 1;
    }

    fclose(file_ptr);
    free(arg);
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec end_cpu;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);


    return (void*) get_time_micro_s(start, end);
}

int main(int charc, char* argv []) {
    if (charc < 5) {
        printf("Not enaugh arguments. :(\n");
        return -1;
    }
    struct timespec start_real;
    clock_gettime(CLOCK_REALTIME, &start_real);
    struct timespec start_cpu;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_cpu);

    m = atoi(argv[1]);
    char* flag = argv[2];
    filename = argv[3];
    char* output_fn = argv[4];

    header = read_header(filename);
    pthread_t* threads = malloc(sizeof(pthread_t) * m);
    thread_res = malloc(sizeof(int*) * m);

    if (strcmp(flag, "sign") == 0) {
        int reminder = (header -> M + 1) % m;
        int uniform  = (header -> M + 1) / m;
        int limit = 0;
        //creating threads
        for (int i = 0; i < m; i++) {
           int from = limit;
           int until   = from + uniform;

           if (reminder > 0) {
               until += 1;
               reminder -= 1;
           }
           limit = until;

           SignThreadArg* args = malloc(sizeof(SignThreadArg));
           args -> from = from;
           args -> until = until;
           args -> k = i;

           pthread_create(&threads[i], NULL, thread_sign, (void*) args);
        }

    } else if (strcmp(flag, "block") == 0) {
        for (int i = 0; i < m; i++) {
            ThreadArg* arg = malloc(sizeof(ThreadArg));
            arg -> k = i;

            pthread_create(&threads[i], NULL, thread_block, (void*) arg);
        } 
    } else if (strcmp(flag, "interleaved") == 0) {
        for (int i = 0; i < m; i++) {
            ThreadArg* arg = malloc(sizeof(ThreadArg));
            arg -> k = i;

            pthread_create(&threads[i], NULL, thread_interleaved, (void*) arg);
        } 

    } else {
        printf("Given flag: %s not supported. :(\n", flag);
    }
    printf("============================================================\n");
    printf("Running %s option, with %d threads.\n", flag, m);

    // Wait for threads termination and gather results in a generic way.
    int* results = calloc(header -> M + 1, sizeof(int)); 
    void* thr_return;
    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], &thr_return);
        long int thr_time = (long int) thr_return;
        printf("Thread: %lx, elapsed time: %ld mircoseconds.\n", threads[i], thr_time);
        for (int p = 0; p < header -> M + 1; p++) {
            results[p] += thread_res[i][p];
        }

        free(thread_res[i]);
    }

    save_res(results, header -> M + 1, output_fn);
    printf("\n");

    struct timespec end_real;
    clock_gettime(CLOCK_REALTIME, &end_real);
    struct timespec end_cpu;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_cpu);

    long int cpu_time = get_time_micro_s(start_cpu, end_cpu);
    long int real_time = get_time_micro_s(start_real, end_real);

    printf("Cpu time: %ld microseconds.\n", cpu_time);
    printf("Real time: %ld microseconds.\n", real_time);
    printf("==================================================================\n\n");
    
    free(thread_res);
    free(results);
    free(threads);

    return 0;
}