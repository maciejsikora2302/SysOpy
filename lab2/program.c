#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 



int main(int argc, char **argv) 
{
    srand(time(NULL));

    if(argc < 5){
        printf("Too few arguments :3\n");
        return 1;
    }
    if(argc == 5 && strcmp(argv[1], "generate") == 0){
        char* file_name = argv[2];
        char* number_of_records_to_create_tmp = argv[3];
        int number_of_records_to_create = atoi(number_of_records_to_create_tmp);
        char* length_of_record_tmp = argv[4]; 
        int length_of_record = atoi(length_of_record_tmp); 
        printf("File name: %s\nRecords to create: %d\nLength: %d\n", file_name, number_of_records_to_create, length_of_record);
        //Do tąd jak na razie cycuś glancuś pizdeczka
        
        FILE* file = fopen(file_name, "w");
        for(int i = 0; i < number_of_records_to_create; i++){
            char* mini_buffor = (char*) calloc(1, sizeof(char));
            for(int j = 0; j < length_of_record; j++){
                
                int ran = rand()%26;
                if(rand()%2 == 0){
                    sprintf(mini_buffor, "%c", ran + 65); //Duże litery
                }else{
                    sprintf(mini_buffor, "%c", ran + 97); //Małe litery
                }

                fwrite(mini_buffor, 1, 1, file);
            }
            fwrite("\n", 1, 1, file);
        }


        fclose(file);










    }else if(argc == 6 &&  strcmp(argv[1], "sort") == 0 ){

    }else if(argc == 6 && strcmp(argv[1], "copy") == 0){

    }else{
        printf("Wrong command :3\n");
    }


}