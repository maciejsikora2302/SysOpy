#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 

int partition(FILE* file, int from, int to, int char_to_compare){
    return 0;
}


void qsort_file(FILE* file, int from, int to, int char_to_compare){

    int partition_index = partition(file, from, to, char_to_compare);

    qsort_file(file, from, partition_index - 1, char_to_compare);
    qsort_file(file, partition_index + 1, to, char_to_compare);
}




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
        char* file_name = argv[2];
        
        char* number_of_records_to_create_tmp = argv[3];
        int number_of_records_to_create = atoi(number_of_records_to_create_tmp);
        
        char* length_of_record_tmp = argv[4]; 
        int length_of_record = atoi(length_of_record_tmp); 

        char* source = argv[5]; 
        
        printf("File name: %s\nRecords: %d\nLength: %d\nSource: %s\n", file_name, number_of_records_to_create, length_of_record, source);

        
        
        if(strcmp(source, "lib") == 0){
            FILE* file = fopen(file_name, "r+");

            // char* line_buffor = (char*) acalloc()
            // fpos_t tmp;
            // fgetpos(file, &tmp);
            printf("%ld\n", ftell(file));



            fclose(file);
        }else if(strcmp(source, "sys") == 0){

        }
        
        
        // for(int i = 0; i < number_of_records_to_create; i++){
        //     char* mini_buffor = (char*) calloc(1, sizeof(char));
        //     for(int j = 0; j < length_of_record; j++){
                
        //         int ran = rand()%26;
        //         if(rand()%2 == 0){
        //             sprintf(mini_buffor, "%c", ran + 65); //Duże litery
        //         }else{
        //             sprintf(mini_buffor, "%c", ran + 97); //Małe litery
        //         }

        //         fwrite(mini_buffor, 1, 1, file);
        //     }

        //     fwrite("\n", 1, 1, file);
        // }
        // fclose(file);
    }else if(argc == 6 && strcmp(argv[1], "copy") == 0){

    }else{
        printf("Wrong command :3\n");
    }


}