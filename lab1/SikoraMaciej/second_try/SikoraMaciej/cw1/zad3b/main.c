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

double time_diff(clock_t t1, clock_t t2){
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void print_res(const char* command, clock_t start, clock_t end, struct tms* t_start, struct tms* t_end){
    printf("=======================================\n");
    printf("Command: %s\n", command);
    printf("Czas rzeczywisty: %fl\n", time_diff(start, end));
    printf("Czas użytkownika: %fl\n", time_diff(t_start -> tms_utime, t_end -> tms_utime));
    printf("Czas systemowy: %fl\n", time_diff(t_start -> tms_stime, t_end -> tms_stime));

    fprintf(result_file, "=======================================\n");
    fprintf(result_file, "Command: %s\n", command);
    fprintf(result_file, "Czas rzeczywisty: %fl\n", time_diff(start, end));
    fprintf(result_file, "Czas użytkownika: %fl\n", time_diff(t_start -> tms_utime, t_end -> tms_utime));
    fprintf(result_file, "Czas systemowy: %fl\n", time_diff(t_start -> tms_stime, t_end -> tms_stime));
}

int main(int argc, char* argv[]) {

    result_file = fopen("results3b.txt", "a");

    clock_t startTime;
    clock_t endTime;
    struct tms* tmsStart = calloc(1, sizeof(struct tms*));
    struct tms* tmsEnd   = calloc(1, sizeof(struct tms*));

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "create_table") == 0){
            startTime = times(tmsStart);
            create_main_table(atoi(argv[++i]));
            endTime = times(tmsEnd);
            print_res("create_table", startTime, endTime, tmsStart, tmsEnd);
        }else if(strcmp(argv[i], "compare_pairs") == 0){
            startTime = times(tmsStart);
            char** pairs = convert_file_names(argv[++i]);
            for(int j=0; j<number_of_files; j++){
                // printf("File1 and file2: %s %s\n", pairs[j], pairs[j+1]);

                invoke_diff(pairs[j], pairs[j+1]);
                j++;

                create_boo_from_txt("tmp.txt");
                // printf("Index of this boo: %d\n", index);
                // printf("Created operations: %d\n", get_number_of_operations(index));
            }
            endTime = times(tmsEnd);
            print_res("compare_pairs", startTime, endTime, tmsStart, tmsEnd);
        }else if(strcmp(argv[i], "remove_block") == 0){
            startTime = times(tmsStart);
            int block_nr = atoi(argv[++i]);
            delete_boo(block_nr);
            endTime = times(tmsEnd);
            print_res("remove_block", startTime, endTime, tmsStart, tmsEnd);
        }else if(strcmp(argv[i], "remove_operation") == 0){
            startTime = times(tmsStart);
            int block_nr = atoi(argv[++i]);
            int op_nr = atoi(argv[++i]);
            delete_o_in_boo(block_nr, op_nr);
            endTime = times(tmsEnd);
            print_res("remove_operation", startTime, endTime, tmsStart, tmsEnd);
        }else if(strcmp(argv[i], "add_and_remove") == 0){
            startTime = times(tmsStart);
            int times_to_repeat = atoi(argv[++i]);
            int number_of_files = atoi(argv[++i]);
            create_main_table(number_of_files);
            char** pairs = convert_file_names(argv[++i]);
            for(int j=0; j<times_to_repeat; j++){
                invoke_diff(pairs[0], pairs[1]);

                create_boo_from_txt("tmp.txt");
                // printf("Index of this boo: %d\n", index);
                // printf("Created operations: %d\n", get_number_of_operations(index));
                next_free_slot--;
                delete_boo(next_free_slot);
            }
            endTime = times(tmsEnd);
            print_res("add_and_remove", startTime, endTime, tmsStart, tmsEnd);
        }else{
            printf("Command not recognised: %s\n", argv[i]);
        }
    }

    fclose(result_file);

    return 0;
}
