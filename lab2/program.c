#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 

int partition(FILE* file, int from, int to, int char_to_compare, int buffer_size){
    int pivot_index = from;
    int lower_then_pivot_index = from;
    for(int i = from + 1; i < to; i++){
        char* buffer_for_pivot = (char*) calloc(buffer_size, sizeof(char));

        if(fseek(file, pivot_index, SEEK_SET) == 0){
            fread(buffer_for_pivot, 1, buffer_size, file);
        }else{
            printf("Something went wrong with fseek in qsort for pivot\n");
            perror("Something went wrong with fseek in qsort for pivot\n");
            return -1;
        }

        char* buffer_for_str = (char*) calloc(buffer_size, sizeof(char));
        
        if(fseek(file, i, SEEK_SET) == 0){
            fread(buffer_for_str, 1, buffer_size, file);
        }else{
            printf("Something went wrong with fseek in qsort buffer for str\n");
            perror("Something went wrong with fseek in qsort buffer for str\n");
            return -1;
        }


        if (buffer_for_str[char_to_compare] < buffer_for_pivot[char_to_compare]){
            
            
            if(fseek(file, lower_then_pivot_index, SEEK_SET) == 0){
                fread(buffer_for_pivot, 1, buffer_size, file); //overwrite to not use free and calloc again
                fwrite(buffer_for_str, 1, buffer_size, file);

                if(fseek(file, i, SEEK_SET) == 0){
                    fwrite(buffer_for_pivot, 1, buffer_size, file);
                }else{
                    printf("Something went wrong with fseek in qsort in\n");
                    perror("Something went wrong with fseek in qsort in\n");
                    return -1;
                }

            }else{
                printf("Something went wrong with fseek in qsort out\n");
                perror("Something went wrong with fseek in qsort out\n");
                return -1;
            }
            
            lower_then_pivot_index++;
        }
        free(buffer_for_pivot);
        free(buffer_for_str);
    }


    char* buffer_for_pivot = (char*) calloc(buffer_size, sizeof(char));

    if(fseek(file, pivot_index, SEEK_SET) == 0){
        fread(buffer_for_pivot, 1, buffer_size, file);
    }else{
        printf("Something went wrong with fseek in qsort for pivot\n");
        perror("Something went wrong with fseek in qsort for pivot\n");
        return -1;
    }

    char* buffer_for_str = (char*) calloc(buffer_size, sizeof(char));
    
    if(fseek(file, lower_then_pivot_index, SEEK_SET) == 0){
        fread(buffer_for_str, 1, buffer_size, file);
        fwrite(buffer_for_pivot, 1 ,buffer_size, file);
    }else{
        printf("Something went wrong with fseek in qsort buffer for str\n");
        perror("Something went wrong with fseek in qsort buffer for str\n");
        return -1;
    }

    if(fseek(file, pivot_index, SEEK_SET) == 0){
        fwrite(buffer_for_pivot, 1, buffer_size, file);
    }else{
        printf("Something went wrong with fseek in qsort for pivot\n");
        perror("Something went wrong with fseek in qsort for pivot\n");
        return -1;
    }


    return lower_then_pivot_index;
}


void qsort_file(FILE* file, int from, int to, int char_to_compare, int buffer_size){

    int partition_index = partition(file, from, to, char_to_compare, buffer_size);

    qsort_file(file, from, partition_index - 1, char_to_compare, buffer_size);
    qsort_file(file, partition_index + 1, to, char_to_compare, buffer_size);
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