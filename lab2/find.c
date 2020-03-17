#define _XOPEN_SOURCE 500

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
#include <ftw.h>
#include <stdint.h>

// int MAXLEVEL = 100000;



int mtime_flag = 0;
int atime_flag = 0;
int maxdepth_flag = 0;

int mtime_value = 0;
int mtime_sign = 0;

int atime_value = 0;
int atime_sign = 0;

int maxdepth_value = 0;

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

void print_info(char* path, struct stat* statistics){
    time_t seconds = time(NULL);
    printf("File path: %s\n", path);

    // printf("Depth and maxdepth: %d, %d\n", current_depth, maxdepth_value);

    printf("Number of hard links: %ld\n", statistics -> st_nlink);

    if(S_ISREG(statistics->st_mode)){
        printf("File type: %s\n", "file");
    }else if(S_ISDIR(statistics->st_mode)){
        printf("File type: %s\n", "dir");
    }else if(S_ISCHR(statistics->st_mode)){
        printf("File type: %s\n", "char dev");
    }else if(S_ISBLK(statistics->st_mode)){
        printf("File type: %s\n", "block dev");
    }else if(S_ISFIFO(statistics->st_mode)){
        printf("File type: %s\n", "fifo");
    }else if(S_ISLNK(statistics->st_mode)){
        printf("File type: %s\n", "slink");
    }else if(S_ISSOCK(statistics->st_mode)){
        printf("File type: %s\n", "sock");
    }

    printf("Size in bytes: %ld\n", statistics -> st_size);

    printf("Last access (years, months, days, hours): %ld, %ld, %ld, %ld\n", (seconds - statistics->st_atime)/31104000, (seconds - statistics->st_atime)/2592000, (seconds - statistics->st_atime)/86400, (seconds - statistics->st_atime)/3600);

    printf("Last modification (years, months, days, hours): %ld, %ld, %ld, %ld\n", (seconds - statistics->st_mtime)/31104000, (seconds - statistics->st_mtime)/2592000, (seconds - statistics->st_mtime)/86400, (seconds - statistics->st_mtime)/3600);
    
    printf("\n");
}

void seach_directory(char* path, int current_depth, int mtime_flag, int atime_flag, int maxdepth_flag, int mtime_sign, int atime_sign, int mtime_value, int atime_value, int maxdepth_value){
    time_t seconds = time(NULL);

    DIR* dir;
    struct dirent* entry;

    if((dir = opendir(path))){
        // printf("%s", "tu dziala\n");
        while((entry = readdir(dir)) != NULL){
            // printf("%s", "tu dziala\n");
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if(current_depth == -1){
                current_depth++;
                struct stat* tmp = (struct stat*) calloc(1, sizeof(struct stat));
                lstat(path, tmp);
                print_info(path, tmp);
                free(tmp);
                if(maxdepth_flag == 1 && maxdepth_value == 0){
                    // current_depth++;
                    return;
                }
            }
            struct stat* tmp = (struct stat*) calloc(1, sizeof(struct stat));
            char* tmp_path = (char*) calloc(1000, sizeof(char));
            strcpy(tmp_path, path);
            strcat(tmp_path, "/");
            strcat(tmp_path, entry->d_name);
            lstat(tmp_path, tmp);

            if(mtime_flag){
                
                int mtime = (seconds - tmp->st_mtime)/86400;
                int days = mtime_value;
                if(mtime_sign == 1 && !(mtime > days)){ 
                    free(tmp_path);
                    free(tmp);                                          //written in this form so that it looks clearer
                    continue;                                           //could have written ine one linke but then it is very hard to understand
                }else if(mtime_sign == 0 && !(mtime == days)){
                    free(tmp_path);
                    free(tmp); 
                    continue;
                }else if(mtime_sign == -1 && !(mtime < days)){
                    free(tmp_path);
                    free(tmp); 
                    continue;
                }
            }


            if(atime_flag){
                int atime = (seconds - tmp->st_atime)/86400;
                int days = atime_value;
                if(atime_sign == 1 && !(atime > days)){ 
                    free(tmp_path);
                    free(tmp);                                          //written in this form so that it looks clearer
                    continue;                                           //could have written ine one linke but then it is very hard to understand
                }else if(atime_sign == 0 && !(atime == days)){
                    free(tmp_path);
                    free(tmp);                          
                    continue;
                }else if(atime_sign == -1 && !(atime < days)){
                    free(tmp_path);
                    free(tmp);                          
                    continue;
                }
            }


            char* actualpath = (char*) calloc(10000, sizeof(char));
            realpath(tmp_path, actualpath);
            
            print_info(actualpath, tmp);
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
                if(maxdepth_flag == 1 && (current_depth + 1 < maxdepth_value)){
                    current_depth++;
                    seach_directory(actualpath, current_depth, mtime_flag, atime_flag, maxdepth_flag, mtime_sign, atime_sign, mtime_value, atime_value, maxdepth_value);
                }else if(maxdepth_flag == 0){
                    seach_directory(actualpath, current_depth, mtime_flag, atime_flag, maxdepth_flag, mtime_sign, atime_sign, mtime_value, atime_value, maxdepth_value);
                }
                    
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



static int display_info(const char *path, const struct stat *statistics, int tflag, struct FTW *ftwbuf){
    // printf("%-3s %2d %7jd   %-40s %d %s\n",
    //                (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
    //                (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?
    //                    (S_ISBLK(sb->st_mode) ? "f b" :
    //                     S_ISCHR(sb->st_mode) ? "f c" :
    //                     S_ISFIFO(sb->st_mode) ? "f p" :
    //                     S_ISREG(sb->st_mode) ? "f r" :
    //                     S_ISSOCK(sb->st_mode) ? "f s" : "f ?") :
    //                (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
    //                (tflag == FTW_SLN) ? "sln" : "?",
    //                ftwbuf->level, (intmax_t) sb->st_size,
    //                fpath, ftwbuf->base, fpath + ftwbuf->base);
    //            return 0;



    if(maxdepth_flag == 1 && ftwbuf -> level > maxdepth_value){
        return 0;
    }
    time_t seconds = time(NULL);

    if(mtime_flag){       
        int mtime = (seconds - statistics->st_mtime)/86400;
        int days = mtime_value;
        if(mtime_sign == 1 && !(mtime > days)){ 
            return 0;                                          
        }else if(mtime_sign == 0 && !(mtime == days)){
            return 0;
        }else if(mtime_sign == -1 && !(mtime < days)){
            return 0;
        }
    }


    if(atime_flag){
        int atime = (seconds - statistics->st_atime)/86400;
        int days = atime_value;
        if(atime_sign == 1 && !(atime > days)){ 
            return 0;
        }else if(atime_sign == 0 && !(atime == days)){
            return 0;
        }else if(atime_sign == -1 && !(atime < days)){
            return 0;
        }
    }

    
    printf("File path: %s\n", path);

    // printf("Depth and maxdepth: %d, %d\n", current_depth, maxdepth_value);

    printf("Number of hard links: %ld\n", statistics -> st_nlink);

    if(S_ISREG(statistics->st_mode)){
        printf("File type: %s\n", "file");
    }else if(S_ISDIR(statistics->st_mode)){
        printf("File type: %s\n", "dir");
    }else if(S_ISCHR(statistics->st_mode)){
        printf("File type: %s\n", "char dev");
    }else if(S_ISBLK(statistics->st_mode)){
        printf("File type: %s\n", "block dev");
    }else if(S_ISFIFO(statistics->st_mode)){
        printf("File type: %s\n", "fifo");
    }else if(S_ISLNK(statistics->st_mode)){
        printf("File type: %s\n", "slink");
    }else if(S_ISSOCK(statistics->st_mode)){
        printf("File type: %s\n", "sock");
    }

    printf("Size in bytes: %ld\n", statistics -> st_size);

    printf("Last access (years, months, days, hours): %ld, %ld, %ld, %ld\n", (seconds - statistics->st_atime)/31104000, (seconds - statistics->st_atime)/2592000, (seconds - statistics->st_atime)/86400, (seconds - statistics->st_atime)/3600);

    printf("Last modification (years, months, days, hours): %ld, %ld, %ld, %ld\n", (seconds - statistics->st_mtime)/31104000, (seconds - statistics->st_mtime)/2592000, (seconds - statistics->st_mtime)/86400, (seconds - statistics->st_mtime)/3600);
    
    printf("\n");

    return 0;
}

int main(int argc, char **argv){
    
    
    // printf("%ld\n", (seconds - tmp->st_atime)/3600);
    printf("\n");



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

    int flags = 0;
    flags |= FTW_PHYS;
    if (nftw(path, display_info, 20, flags) == -1){
        perror("nftw");
        exit(EXIT_FAILURE);
    }


    // seach_directory(path, -1, mtime_flag, atime_flag, maxdepth_flag, mtime_sign, atime_sign, mtime_value, atime_value, maxdepth_value);

    // listdir("..", 0);
}