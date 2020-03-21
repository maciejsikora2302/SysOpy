#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <time.h>
#include <memory.h>
#include <sys/times.h>
 

int partition_lib(FILE* file, int from, int to, int char_to_compare, int buffer_size){
    int pivot_index = from;
    int lower_then_pivot_index = from;
    for(int i = from + buffer_size; i < to; i += buffer_size){
        char* buffer_for_pivot = (char*) calloc(buffer_size, sizeof(char));

        if(fseek(file, pivot_index, 0) == 0){
            fread(buffer_for_pivot, 1, buffer_size, file);
        }else{
            printf("Something went wrong with fseek in qsort for pivot lib\n");
            return -1;
        }

        char* buffer_for_str = (char*) calloc(buffer_size, sizeof(char));
        
        if(fseek(file, i, 0) == 0){
            fread(buffer_for_str, 1, buffer_size, file);
        }else{
            printf("Something went wrong with fseek in qsort buffer for str\n");
            return -1;
        }
        if (buffer_for_str[char_to_compare] < buffer_for_pivot[char_to_compare]){
            
            lower_then_pivot_index += buffer_size;
            if(fseek(file, lower_then_pivot_index, 0) == 0){
                fread(buffer_for_pivot, 1, buffer_size, file); //overwrite to not use free and calloc again
                fseek(file, lower_then_pivot_index, 0);
                fwrite(buffer_for_str, 1, buffer_size, file);

                if(fseek(file, i, 0) == 0){
                    fwrite(buffer_for_pivot, 1, buffer_size, file);
                }else{
                    printf("Something went wrong with fseek in qsort in\n");
                    return -1;
                }

            }else{
                printf("Something went wrong with fseek in qsort out\n");
                return -1;
            }
            
            
        }

        free(buffer_for_pivot);
        free(buffer_for_str);
    }


    char* buffer_for_pivot = (char*) calloc(buffer_size, sizeof(char));

    if(fseek(file, pivot_index, 0) == 0){
        fread(buffer_for_pivot, 1, buffer_size, file);
    }else{
        printf("Something went wrong with fseek in qsort for 6345gdfg234\n");
        return -1;
    }

    char* buffer_for_str = (char*) calloc(buffer_size, sizeof(char));
    
    if(fseek(file, lower_then_pivot_index, 0) == 0){
        fread(buffer_for_str, 1, buffer_size, file);
        fseek(file, lower_then_pivot_index, 0);
        fwrite(buffer_for_pivot, 1 ,buffer_size, file);
    }else{
        printf("Something went wrong with fseek in qsort buffer for st23r\n");
        return -1;
    }

    if(fseek(file, pivot_index, 0) == 0){
        fwrite(buffer_for_str, 1, buffer_size, file);
    }else{
        printf("Something went wrong with fseek in qsort for 123452346123462\n");
        return -1;
    }

    return lower_then_pivot_index;
}


void qsort_fil_lib(FILE* file, int from, int to, int char_to_compare, int buffer_size){

    if(from < to){
        int partition_index = partition_lib(file, from, to, char_to_compare, buffer_size);

        qsort_fil_lib(file, from, partition_index - buffer_size, char_to_compare, buffer_size);
        qsort_fil_lib(file, partition_index + buffer_size, to, char_to_compare, buffer_size);
    }
}


int partition_sys(int fd, int from, int to, int char_to_compare, int buffer_size){
    int pivot_index = from;
    int lower_then_pivot_index = from;
    for(int i = from + buffer_size; i < to; i += buffer_size){
        char* buffer_for_pivot = (char*) calloc(buffer_size, sizeof(char));

        // printf("%d", lseek(fd, pivot_index, SEEK_SET));
        // read(fd, buffer_for_pivot, buffer_size);
        // puts(buffer_for_pivot);
        // free(buffer_for_pivot);
        // return -1;


        if(lseek(fd, pivot_index, SEEK_SET) >= 0){
            read(fd, buffer_for_pivot, buffer_size);
        }else{
            // printf("%ld", lseek(fd, pivot_index, SEEK_SET));
            perror("Something went wrong with fseek in qsort for pivot systemowo\n");
            return -1;
        }

        char* buffer_for_str = (char*) calloc(buffer_size, sizeof(char));
        
        if(lseek(fd, i, SEEK_SET) >= 0){
            read(fd, buffer_for_str, buffer_size);
        }else{
            printf("Something went wrong with fseek in qsort buffer for str\n");
            return -1;
        }
        if (buffer_for_str[char_to_compare] < buffer_for_pivot[char_to_compare]){
            
            lower_then_pivot_index += buffer_size;
            if(lseek(fd, lower_then_pivot_index, SEEK_SET) >= 0){
                read(fd, buffer_for_pivot, buffer_size);
                lseek(fd, lower_then_pivot_index, SEEK_SET);
                write(fd, buffer_for_str, buffer_size);

                if(lseek(fd, i, SEEK_SET) >= 0){
                    write(fd, buffer_for_pivot, buffer_size);
                }else{
                    printf("Something went wrong with fseek in qsort in\n");
                    return -1;
                }

            }else{
                printf("Something went wrong with fseek in qsort out\n");
                return -1;
            }
            
            
        }

        free(buffer_for_pivot);
        free(buffer_for_str);
    }


    char* buffer_for_pivot = (char*) calloc(buffer_size, sizeof(char));

    if(lseek(fd, pivot_index, SEEK_SET) >= 0){
        read(fd, buffer_for_pivot, buffer_size);
    }else{
        printf("Something went wrong with fseek in qsort for 123\n");
        return -1;
    }

    char* buffer_for_str = (char*) calloc(buffer_size, sizeof(char));
    
    if(lseek(fd, lower_then_pivot_index, SEEK_SET) >= 0){
        read(fd, buffer_for_str, buffer_size);
        lseek(fd, lower_then_pivot_index, SEEK_SET);
        write(fd, buffer_for_pivot, buffer_size);
    }else{
        printf("Something went wrong with fseek in qsort buffer for 5342\n");
        return -1;
    }

    if(lseek(fd, pivot_index, SEEK_SET) >= 0){
        write(fd, buffer_for_str, buffer_size);
    }else{
        printf("Something went wrong with fseek in qsort for 162341234\n");
        return -1;
    }

    return lower_then_pivot_index;

}


void qsort_fil_sys(int fd, int from, int to, int char_to_compare, int buffer_size){

    if(from < to){
        int partition_index = partition_sys(fd, from, to, char_to_compare, buffer_size);

        if(partition_index < 0) return;

        qsort_fil_sys(fd, from, partition_index - buffer_size, char_to_compare, buffer_size);
        qsort_fil_sys(fd, partition_index + buffer_size, to, char_to_compare, buffer_size);
    }
}

double time_diff(clock_t t1, clock_t t2){
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void write_time_to_file(char* message, char* file_name, clock_t start, clock_t end, struct tms* t_start, struct tms* t_end){
    double real = time_diff(start, end);
    double user = time_diff(t_start -> tms_utime, t_end -> tms_utime);
    double system = time_diff(t_start -> tms_stime, t_end -> tms_stime);

    FILE* file = fopen(file_name, "a+");
    fprintf(file, "%s", message);
    fprintf(file, "%s", "Real time: ");
    fprintf(file, "%f", real);
    fprintf(file, "%s", "\nUser time: ");
    fprintf(file, "%f", user);
    fprintf(file, "%s", "\nSystem time: ");
    fprintf(file, "%f", system);
    fprintf(file, "%s", "\n\n==========================================\n\n");
    fclose(file);
}


int main(int argc, char **argv) 
{
    srand(time(NULL));

    clock_t start;
    clock_t end;

    struct tms* tms_start = calloc(1, sizeof(struct tms*));
    struct tms* tms_end   = calloc(1, sizeof(struct tms*));

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

        char* message = (char*) calloc(250, sizeof(char));
        
        sprintf(message, "Command: %s\nFile name: %s\nRecords: %d\nLength: %d\nSource: %s\n", "sort", file_name, number_of_records_to_create, length_of_record, source);

        
        
        if(strcmp(source, "lib") == 0){
            start = times(tms_start);
            FILE* file = fopen(file_name, "r+");
            for(int i = length_of_record - 1; i>=0; i-- )
                qsort_fil_lib(file, 0, number_of_records_to_create * (length_of_record+1), i, length_of_record+1);
            fclose(file);
            end = times(tms_end);
        }else if(strcmp(source, "sys") == 0){
            start = times(tms_start);
            int fd = open(file_name, O_RDWR);
            if(fd < 0 ) perror("Plik sie nie otworzył");
            for(int i = length_of_record - 1; i>=0; i-- )
                qsort_fil_sys(fd, 0, number_of_records_to_create * (length_of_record+1), i, length_of_record+1);
            close(fd);
            end = times(tms_end);
        }
        write_time_to_file(message, "wyniki.txt", start, end, tms_start, tms_end);
    }else if(argc == 7 && strcmp(argv[1], "copy") == 0){
        char* file_name_from = argv[2];

        char* file_name_to = argv[3];

        char* number_of_records_to_create_tmp = argv[4];
        int number_of_records_to_create = atoi(number_of_records_to_create_tmp);

        char* length_of_record_tmp = argv[5]; 
        int length_of_record = atoi(length_of_record_tmp); 

        char* source = argv[6]; 

        char* message = (char*) calloc(250, sizeof(char));
        sprintf(message, "Command: %s\nFile name from: %s\nFile name to: %s\nRecords to create: %d\nLength: %d\nSource: %s\n", "copy", file_name_from, file_name_to, number_of_records_to_create, length_of_record, source);


        if(strcmp(source, "lib")  == 0){
            start = times(tms_start);
            FILE* file_from = fopen(file_name_from, "r");
            FILE* file_to = fopen(file_name_to, "w");
            for(int i=0;i<number_of_records_to_create;i++){
                char* buffor = (char*) calloc(length_of_record+1, sizeof(char));
                
                fread(buffor, 1, length_of_record + 1, file_from);
                fwrite(buffor, 1 ,length_of_record + 1, file_to);

                free(buffor);
            }
            fclose(file_to);
            fclose(file_from);
            end = times(tms_end);
        }else if (strcmp(source, "sys") == 0){
            start = times(tms_start);
            length_of_record++;
            char* buffor = (char*) calloc(length_of_record, sizeof(char));
            int we, wy;
            we = open(file_name_from, O_RDONLY);
            wy = open(file_name_to, O_WRONLY|O_CREAT);
            while(read(we, buffor, length_of_record) > 0)
                write(wy, buffor, length_of_record); 
            free(buffor);
            end = times(tms_end);
        }
        write_time_to_file(message, "wyniki.txt", start, end, tms_start, tms_end);

    }else{
        printf("Wrong command :3\n");
    }


}