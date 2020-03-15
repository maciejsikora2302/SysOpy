#include "lib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/times.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>

FILE* result_file = NULL;

struct block_of_operations** main_table = NULL;

void error(const char* msg) {
    printf("%s \n", msg);
    exit(-1);
}

double time_diff(clock_t t1, clock_t t2){
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void print_res(const char* msg, clock_t start, clock_t end, struct tms* t_start, struct tms* t_end){
    printf("%s --------------\n", msg);
    printf("\tREAL_TIME: %fl\n", time_diff(start, end));
    printf("\tUSER_TIME: %fl\n", time_diff(t_start -> tms_utime, t_end -> tms_utime));
    printf("\tSYSTEM_TIME: %fl\n", time_diff(t_start -> tms_stime, t_end -> tms_stime));

    fprintf(result_file, "%s --------------\n", msg);
    fprintf(result_file, "\tREAL_TIME: %fl\n", time_diff(start, end));
    fprintf(result_file, "\tUSER_TIME: %fl\n", time_diff(t_start -> tms_utime, t_end -> tms_utime));
    fprintf(result_file, "\tSYSTEM_TIME: %fl\n\n", time_diff(t_start -> tms_stime, t_end -> tms_stime));
}

char* create_table_test(char* sizeStr) {
    int size = atoi(sizeStr);
    char* msg = "create_table ";
    msg = concat(msg, sizeStr);
    main_table = give_only_main_table(size);

    return msg;
}

void compare_pairs_test(char* file_names, char* number_of_files) {
    int nof = atoi(number_of_files);
    if(main_table != NULL)
        free(main_table);
    main_table = give_main_table(file_names, nof);
}

char* remove_block_test(char* index_c) {
    int index = atoi(index_c);
    char* result = concat_w_sep("Remove_block ", index_c, " ");

    delete_boo(main_table, index);

    return result;
}

char* remove_operation_test(char* block_index_c, char* op_index_c) {
    int block_index = atoi(block_index_c);
    int op_index = atoi(op_index_c);
    char* msg = concat_w_sep("Remove operation ", block_index_c, " ");
    msg = concat_w_sep(msg, op_index_c, " ");

    delete_o_in_boo(main_table[block_index], op_index);

    return msg;
}

int main(int argc, char* argv[]) {
    result_file = fopen("raport2.txt", "a");

    // printf("%d\n", argc);

    int i = 1;
    clock_t startTime;
    clock_t endTime;
    struct tms* tmsStart = calloc(1, sizeof(struct tms*));
    struct tms* tmsEnd   = calloc(1, sizeof(struct tms*));

    while (i < argc) {
        const char* command = argv[i];
        char* msg = NULL;
        startTime = times(tmsStart);

        if (strcmp("create_table", command) == 0) {
            msg = create_table_test(argv[++i]);
        } else if (strcmp("compare_pairs", command) == 0) {
            msg = concat_w_sep("compare_pairs", argv[i + 1], " ");
            msg = concat_w_sep("compare_pairs", argv[i + 2], " ");
            
            // int filesCount = atoi(argv[++i]);
            compare_pairs_test(argv[i+2], argv[i+1]);
            i += 2;
            // i += filesCount;           
        } else if (strcmp("remove_block", command) == 0) {
            msg = remove_block_test(argv[++i]);
        } else if (strcmp("remove_operation", command) == 0) {
            msg = remove_operation_test(argv[i + 1], argv[i + 2]);
            i += 2;
        } else {
            char* tmp = "Command does not exists -> ";

            error(concat_w_sep(tmp, command, " "));
        }

        endTime = times(tmsEnd);
        print_res(msg, startTime, endTime, tmsStart, tmsEnd);
        i++;
    }

    fclose(result_file);

    return 0;
}
