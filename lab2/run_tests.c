#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    system("rm wyniki.txt");
    system("touch wyniki.txt");
    char* command = (char*) calloc(1000, sizeof(char));
    char* program = "./program ";
    char* commands[3] = {"generate ", "copy ", "sort "};
    char* number_of_records[12] = {  "10 ", "20 ", 
                                    "10 ", "20 ", 
                                    "10 ", "20 ",
                                    "10 ", "20 ",
                                    "10 ", "20 ",
                                    "10 ", "20 "};
    char* buff_sizes[6] = {"1 ", "4 ", "512 ", "1024 ", "4096 ", "8192 "};
    for(int i=0; i<6; i++){
        for(int k=0; k<2; k++){
            //generate
            strcpy(command, program);
            strcat(command, commands[0]);
            strcat(command, "generated_data ");
            strcat(command, number_of_records[2 * i + k]);
            strcat(command, buff_sizes[i]);
            system(command);
            //copy1
            strcpy(command, program);
            strcat(command, commands[1]);
            strcat(command, "generated_data ");
            strcat(command, "to_sort_sys ");
            strcat(command, number_of_records[2 * i + k]);
            strcat(command, buff_sizes[i]);
            strcat(command, "sys");
            system(command);
            system("chmod +rwx to_sort_sys");
            //copy2
            strcpy(command, program);
            strcat(command, commands[1]);
            strcat(command, "generated_data ");
            strcat(command, "to_sort_lib ");
            strcat(command, number_of_records[2 * i + k]);
            strcat(command, buff_sizes[i]);
            strcat(command, "lib");
            system(command);
            //sort1
            strcpy(command, program);
            strcat(command, commands[2]);
            strcat(command, "to_sort_sys ");
            strcat(command, number_of_records[2 * i + k]);
            strcat(command, buff_sizes[i]);
            strcat(command, "sys");
            system(command);
            //sort2
            strcpy(command, program);
            strcat(command, commands[2]);
            strcat(command, "to_sort_lib ");
            strcat(command, number_of_records[2 * i + k]);
            strcat(command, buff_sizes[i]);
            strcat(command, "lib");
            system(command);
        }
    }
    free(command);
}