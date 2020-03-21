#define MAX_STRING 100000

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h> 

int number_of_files;

struct block_of_operations** main_tab;

int next_free_slot;

struct block_of_operations{
    int number_of_operations;
    char** operations;
};


bool is_a_number(char character);

void set_single_operation_in_boo(struct block_of_operations *block, char* oper);

int create_boo_from_txt(char* file_name);

char** convert_file_names(char* files);

void invoke_diff(char* first_file, char* second_file);

void create_main_table(int number_of_files);

int get_number_of_operations(int boo_index);

void delete_o_in_boo(int boo_index, int operation_index);

void delete_boo(int boo_index);

char* concat(const char* s1, const char* s2);

char* concat_w_sep(const char* s1, const char* s2, const char* separator);