#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>   
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

void listdir(const char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            listdir(path, indent + 2);
        } else {
            printf("%*s- %s\n", indent, "", entry->d_name);
        }
    }
    closedir(dir);
}

void operaten_on_time(char* parametr, int* time_value, int* time_sign){
        if(parametr[0] == '+'){
                *time_sign = 1;
                char* buffor = (char*) calloc(100, sizeof(char));
                char c = parametr[1];
                int i=0;
                while(c != '\0'){
                    buffor[i] = c;
                    c = parametr[i+2];
                    i++;
                }
                sscanf(buffor, "%d", time_value);
                free(buffor);
            }else if(parametr[0] == '-'){
                *time_sign = -1;
                char* buffor = (char*) calloc(100, sizeof(char));
                char c = parametr[1];
                int i=0;
                while(c != '\0'){
                    buffor[i] = c;
                    c = parametr[i+2];
                    i++;
                }
                sscanf(buffor, "%d", time_value);
                free(buffor);
            }else{
                char* buffor = (char*) calloc(100, sizeof(char));
                char c = parametr[0];
                int i=0;
                while(c != '\0'){
                    buffor[i] = c;
                    c = parametr[++i];
                }
                sscanf(buffor, "%d", time_value);
                free(buffor);
            }
    }

void parse_command(char* command, char* follow_up, int* mtime_flag, int* atime_flag, int* maxdepth_flag, int* mtime_sign, int* atime_sign, int* mtime_value, int* atime_value, int* maxdepth_value){
    if(strcmp(command, "-mtime") == 0){
            *mtime_flag = 1;
            operaten_on_time(follow_up, mtime_value, mtime_sign);
        }else if(strcmp(command, "-atime") == 0){
            *atime_flag = 1;
            operaten_on_time(follow_up, atime_value, atime_sign);
        }else if(strcmp(command, "-maxdepth") == 0){
            *maxdepth_flag = 1;
            char* buffor = (char*) calloc(100, sizeof(char));
            char c = follow_up[0];
            int i=0;
            while(c != '\0'){
                buffor[i] = c;
                c = follow_up[++i];
            }
            sscanf(buffor, "%d", maxdepth_value);
            free(buffor);
        }
}



void seach_directory(char* path){
    time_t seconds = time(NULL);

    DIR* dir;
    struct dirent* entry;

    if((dir = opendir(path))){
        // printf("%s", "tu dziala\n");
        while((entry = readdir(dir)) != NULL){
            // printf("%s", "tu dziala\n");
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            struct stat* tmp = (struct stat*) calloc(1, sizeof(struct stat));
            char* tmp_path = (char*) calloc(1000, sizeof(char));
            strcpy(tmp_path, path);
            strcat(tmp_path, "/");
            strcat(tmp_path, entry->d_name);
            lstat(tmp_path, tmp);

            char* actualpath = (char*) calloc(10000, sizeof(char));
            realpath(tmp_path, actualpath);
            printf("File path: %s\n", actualpath);

            printf("Number of hard links: %ld\n", tmp -> st_nlink);

            if(S_ISREG(tmp->st_mode)){
                printf("File type: %s\n", "file");
            }else if(S_ISDIR(tmp->st_mode)){
                printf("File type: %s\n", "dir");
            }else if(S_ISCHR(tmp->st_mode)){
                printf("File type: %s\n", "char dev");
            }else if(S_ISBLK(tmp->st_mode)){
                printf("File type: %s\n", "block dev");
            }else if(S_ISFIFO(tmp->st_mode)){
                printf("File type: %s\n", "fifo");
            }else if(S_ISLNK(tmp->st_mode)){
                printf("File type: %s\n", "slink");
            }else if(S_ISSOCK(tmp->st_mode)){
                printf("File type: %s\n", "sock");
            }

            printf("Size in bytes: %ld\n", tmp -> st_size);

            printf("Last access (years, months, days, hours): %ld, %ld, %ld, %ld\n", (seconds - tmp->st_atime)/31104000, (seconds - tmp->st_atime)/2592000, (seconds - tmp->st_atime)/86400, (seconds - tmp->st_atime)/3600);

            printf("Last modification (years, months, days, hours): %ld, %ld, %ld, %ld\n", (seconds - tmp->st_mtime)/31104000, (seconds - tmp->st_mtime)/2592000, (seconds - tmp->st_mtime)/86400, (seconds - tmp->st_mtime)/3600);
            
            printf("\n");

            // if(S_ISLNK(tmp->st_mode)){
            //     if(!S_ISREG(tmp->st_mode)){
            //         seach_directory(actualpath); 
            //     }
            //     return;
            // }else{
            //     if(S_ISDIR(tmp->st_mode)){
            //         seach_directory(actualpath);
            //     }
            // }

            if((S_ISDIR(tmp->st_mode) && !S_ISLNK(tmp->st_mode))){
                // printf("\n---->Inputing path: %s\n", actualpath);
                seach_directory(actualpath);
            }
            free(actualpath);
            free(tmp_path);
            free(tmp);
            
        }
    }else{
        perror("opendir");
        printf("Used path: %s\n\n", path);
    }

    closedir(dir);
}

int main(int argc, char **argv){
    
    
    // printf("%ld\n", (seconds - tmp->st_atime)/3600);
    printf("\n");


    int mtime_flag = 0;
    int atime_flag = 0;
    int maxdepth_flag = 0;

    int mtime_value = 0;
    int mtime_sign = 0;

    int atime_value = 0;
    int atime_sign = 0;

    int maxdepth_value = 0;


    // printf("%d\n", argc);
    char* path;

    if(argc == 2){ //podana tylko ścieżka
        path = argv[1];
    }else if(argc == 4){ //mtime, atime or maxdepth podane
        // printf("%s\n", argv[1]);
        parse_command(argv[1], argv[2], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
        path = argv[3];
    }else if(argc == 6){
        parse_command(argv[1], argv[2], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
        parse_command(argv[3], argv[4], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
        path = argv[5];
    }else if(argc == 8){
        parse_command(argv[1], argv[2], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
        parse_command(argv[3], argv[4], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
        parse_command(argv[5], argv[6], &mtime_flag, &atime_flag, &maxdepth_flag, &mtime_sign, &atime_sign, &mtime_value, &atime_value, &maxdepth_value);
        path = argv[7];
    }else{
        puts("Wrong amount of arguments :3");
        path = NULL;
        return 0;
    }

    printf("%s\n", path);
    printf("flags in order: %d, %d, %d\n", mtime_flag, atime_flag, maxdepth_flag);
    printf("signs in order: %d, %d\n", mtime_sign, atime_sign);
    printf("values in order: %d, %d, %d\n\n", mtime_value, atime_value, maxdepth_value);

    
    seach_directory(path);

    // listdir("..", 0);
}