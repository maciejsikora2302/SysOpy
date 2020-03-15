#define MAX_STRING 100000

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h> 


struct block_of_operations{
    int number_of_operations;
    struct single_operation *operations[];
};


struct single_operation{
    char* operation;
};


bool is_a_number(char character);

void set_single_operation_in_boo(struct block_of_operations *block, char* oper);

struct block_of_operations* create_boo_from_txt(char* file_name);

char** convert_file_names(char* files, int number_of_files);

void invoke_diff(char* first_file, char* second_file);

struct block_of_operations** give_main_table(char* names_of_files, int number_of_files);

struct block_of_operations** give_only_main_table(int number_of_files);

int get_number_of_operations(struct block_of_operations* boo);

void delete_o_in_boo(struct block_of_operations* boo, int numer_of_operation);

void delete_boo(struct block_of_operations** main_table, int numer_of_boo);

char* concat(const char* s1, const char* s2);

char* concat_w_sep(const char* s1, const char* s2, const char* separator);