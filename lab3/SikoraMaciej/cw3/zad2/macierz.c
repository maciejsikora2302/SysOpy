#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define SHARED 0
#define SCATTERED 1


/*

./find './lista' 2 10 shared
scattered
#lista ilość_procesów_potomnych max_czas_życia_w_sekundach wariant_zapisu_wspolny_plik_albo_wiele_malych

*/


int main(int argc, char** argv){
    puts("");
    if(argc == 5){
        char* path_to_list = argv[1];
        int number_of_children = atoi(argv[2]);
        int life_time = atoi(argv[3]);
        int save_mode;
        if(strcmp(argv[4], "shared") == 0){
            save_mode = SHARED;
        }else if (strcmp(argv[4], "scattered") == 0){
            save_mode = SCATTERED;
        }else{
            printf("Wrong mode: %s", argv[4]);
            return 0;
        }


        printf("Path to lista: %s\n", path_to_list);
        printf("Number of children: %d\n", number_of_children);
        printf("Life time in seconds: %d\n", life_time);
        printf("Mode (name, program definition): %s, %d\n", argv[4], save_mode);

    }else{
        printf("Wrong amount of arguments: %d\n", argc);
        return 0;
    }
    puts("");
}