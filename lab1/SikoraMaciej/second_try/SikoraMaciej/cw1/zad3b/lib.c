#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h> 


bool is_a_number(char character){
    return (character >= '0' && character <= '9');
}

void set_single_operation_in_boo(struct block_of_operations *block, char* oper){
    char* operation_to_input = (char*) calloc(strlen(oper), sizeof(char));
    strcpy(operation_to_input, oper);
    block -> operations[block -> number_of_operations] = operation_to_input;
    block -> number_of_operations++;
}

int create_boo_from_txt(char* file_name){

    FILE* fp;
    fp = fopen(file_name, "r");


    char* tmp = (char*) calloc(1000000, sizeof(char));

    if(fp == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct block_of_operations *block = (struct block_of_operations*) calloc(1, sizeof(struct block_of_operations));
    block -> operations = (char**)calloc(100000, sizeof(char*));
    block -> number_of_operations = 0;

    int a = 0;

    bool next_character_is_at_the_begining = false;

    do{
        a = fgetc(fp);
        if(next_character_is_at_the_begining && is_a_number(a)){
            set_single_operation_in_boo(block, tmp);
            strcpy(tmp, "");
            next_character_is_at_the_begining = false;
        }

        if(a == '\0' || a == EOF) continue;
        if(a == '\n'){
            next_character_is_at_the_begining = true;
        }
        char* tmp_c = (char*) calloc(1, sizeof(char));
        char c = (char) a;
        tmp_c[0] = c;
        strcat(tmp, tmp_c);
        free(tmp_c);

    }while(a != EOF);

    set_single_operation_in_boo(block, tmp);
    main_tab[next_free_slot] = block;
    next_free_slot++;
    return next_free_slot-1;

}

char** convert_file_names(char* files){
    // puts(files);
    char file_character = files[0];
    char* tmp = (char*) calloc(MAX_STRING, sizeof(char));

    char** return_char_table = (char**) calloc(number_of_files, sizeof(char*));

    for(int i=0;i<number_of_files;i++){
        return_char_table[i] = (char*)calloc(MAX_STRING, sizeof(char));
    }


    int i = 0, file_no = 0;
    while(file_character != '\0'){
        // char* tmp = (char*) calloc(MAX_STRING, sizeof(char));
        if(file_character == ':' || file_character == ' '){
            char* to_input = (char*)calloc(MAX_STRING, sizeof(char));
            strcpy(to_input, tmp);
            return_char_table[file_no] = to_input;
            file_no++;
            memset(tmp, 0, MAX_STRING);
            // free(tmp);

        }else{
            char* serious_name_for_tmp = (char*)calloc(1, sizeof(char));
            serious_name_for_tmp[0] = file_character;
            strcat(tmp, serious_name_for_tmp);
            free(serious_name_for_tmp);
        }
        i++;
        file_character = files[i];
        if(file_character == '\0'){
            char* to_input = (char*)calloc(MAX_STRING, sizeof(char));
            strcpy(to_input, tmp);
            return_char_table[file_no] = to_input;
            file_no++;
            memset(tmp, 0, MAX_STRING);
            // free(tmp);
        }
    }
    return  return_char_table;
}

void invoke_diff(char* first_file, char* second_file){
    char* command = (char*) calloc(1000000, sizeof(char));
    strcpy(command, "diff ");
    strcat(command, first_file);
    strcat(command, " ");
    strcat(command, second_file);
    strcat(command, " > tmp.txt");
    system(command);
    free(command);
}

void create_main_table(int nr_of_files){
    if(nr_of_files % 2 != 0){
        puts("Files must be in pairs");
        return;
    }

    number_of_files = nr_of_files;
    next_free_slot = 0;

    main_tab = (struct block_of_operations**) calloc(number_of_files/2, sizeof(struct block_of_operations*));
}

int get_number_of_operations(int index){
    return main_tab[index]->number_of_operations;
}

void delete_o_in_boo(int boo_index, int operation_index){
    if(operation_index >= main_tab[boo_index]->number_of_operations){
        puts("You are trying to delete operation that is exeeds this block of operations.\n");
        return;
    }
    if(main_tab[boo_index]->operations[operation_index] != NULL){
        free(main_tab[boo_index]->operations[operation_index]);
        return;
    }else{
        printf("Operation(%d) in block(%d) already deleted.\n", operation_index, boo_index);
    }
    return;
}

void delete_boo(int boo_index){
    if(main_tab[boo_index] != NULL){
        for(int i=0; i<main_tab[boo_index]->number_of_operations; i++){
        delete_o_in_boo(boo_index, i);
        }
        free(main_tab[boo_index]);
    }else{
        printf("Block(%d) already deleted.", boo_index);
    }
    
}

char* concat(const char* s1, const char* s2) {
    int   len    = strlen(s1) + strlen(s2) + 1;
    char* result = (char*) malloc(len);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}

char* concat_w_sep(const char* s1, const char* s2, const char* separator) {
    char* temp   = concat(s1, separator);
    char* result = concat(temp, s2);
    
    free(temp);
    return result;
}