#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h> 


bool is_a_number(char character){
    return (character >= '0' && character <= '9');
}


void set_single_operation_in_boo(struct block_of_operations *block, char* oper){
    struct single_operation* op = (struct single_operation*) calloc(1, sizeof(struct single_operation));
    char* operation_to_input = (char*) calloc(MAX_STRING, sizeof(char));
    strcpy(operation_to_input, oper);
    op -> operation = operation_to_input;
    block -> operations[block -> number_of_operations] = op;
    block -> number_of_operations++;
}

struct block_of_operations* create_boo_from_txt(char* file_name){

    FILE* fp;
    fp = fopen(file_name, "r");


    char* tmp = (char*) calloc(1000000, sizeof(char));

    if(fp == NULL){
        exit(EXIT_FAILURE);
    }

    struct block_of_operations *block = (struct block_of_operations*) calloc(1, sizeof(struct block_of_operations));
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
    
    // fclose(fp);

    set_single_operation_in_boo(block, tmp);

    // free(tmp);

    return block;
}


char** convert_file_names(char* files, int number_of_files){
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
    // puts(first_file);
    // puts(second_file);
    strcpy(command, "diff ");
    strcat(command, first_file);
    strcat(command, " ");
    strcat(command, second_file);
    strcat(command, " > tmp.txt");
    // puts(command);
    system(command);
    free(command);
}

struct block_of_operations** give_main_table(char* names_of_files, int number_of_files){
    if(number_of_files % 2 != 0){
        puts("Files must be in pairs");
        return NULL;
    }
    
    char** names = convert_file_names(names_of_files, number_of_files);

    

    struct block_of_operations** main_table = (struct block_of_operations**) calloc(number_of_files/2, sizeof(struct block_of_operations*));

    for(int i = 0; i < number_of_files; i += 2){
        invoke_diff(names[i], names[i+1]);
        main_table[i/2] = create_boo_from_txt("tmp.txt");
    }

    

    return main_table;
}


struct block_of_operations** give_only_main_table(int number_of_files){
    if(number_of_files % 2 != 0){
        puts("Files must be in pairs");
        return NULL;
    }
    
    // char** names = convert_file_names(names_of_files, number_of_files);

    struct block_of_operations** main_table = (struct block_of_operations**) calloc(number_of_files/2, sizeof(struct block_of_operations*));

    return main_table;
}




int get_number_of_operations(struct block_of_operations* boo){
    return boo->number_of_operations;
}

void delete_o_in_boo(struct block_of_operations* boo, int numer_of_operation){
    if(numer_of_operation >= boo->number_of_operations){
        puts("You are trying to delete operation that is exeeds this boo");
        return;
    }
    if(boo->operations[numer_of_operation] != NULL){
        free(boo->operations[numer_of_operation]->operation);
        free(boo->operations[numer_of_operation]);
        return;
    }
    return;
}

void delete_boo(struct block_of_operations** main_table, int numer_of_boo){
    for(int i=0; i<main_table[numer_of_boo]->number_of_operations; i++){
        delete_o_in_boo(main_table[numer_of_boo], i);
    }
    // free(main_table[numer_of_boo]);
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