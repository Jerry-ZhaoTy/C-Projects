#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/sysinfo.h>

char end_mark = '\0';

// struct used to stored zipped data
typedef struct{
    int *number;
    char *character;
}Zipped;

// struct for thread input
typedef struct{
    char *buffer;
    int start;
    int end;
}inputs;

void *wzip(void *arg) {
    // Getting input info from arg
    inputs *input = (inputs *)arg;
    char *buffer = input->buffer;
    int start = input->start;
    int end = input->end;
    // initialize a Zipped struct to store zipped data
    Zipped *zipped = malloc(sizeof(Zipped));
    zipped->number = malloc(sizeof(int) * (end-start));
    zipped->character = malloc(sizeof(char) * (end-start));
    int index = 0;
    // wzip given text
    char new = '\0';
    char old = '\0';
    int count = 1;
    for (int i = start; i < end; i++){
        if (buffer[i] == '\0' || buffer[i] == ' ') continue;
        old = new;
        new = buffer[i];
        if (old == 0) continue;
        if (old == new) count++;
        else{
            zipped->number[index] = count;
            strncat(zipped->character, &old, 1);
            index++;
            count = 1;
        }
    }
    zipped->number[index] = count;
    strncat(zipped->character, &new, 1);
    strncat(zipped->character, &end_mark, 1);
    pthread_exit(zipped);
    return NULL;
}

int main(int argc, char *argv[]){
    // Check if there is a least an input file
    if (argc < 2) {
        printf("pzip: file1 [file2 ...]\n");
        exit(1);
    }
    // get the number of cpus to decide how many threads to create
    int num_cpu = get_nprocs();
    // variable to store the last pair data of each part of text
    int last_num = 0;
    char last_char = '\0';
    // open and zip given files one by one
    for(int j = 1; j < argc; j++){
        // read data from file
        struct stat s;
        int fd = open(argv[j], O_RDONLY);
        if (fd == -1) continue;
        fstat(fd, &s);
        size_t size = s.st_size;
        char *buffer = (char *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        // deal with sparse file
        char *new_buffer = malloc(sizeof(char)*size);
        if (size < 100000000 && size != strlen(buffer)){
            for(int i = 0; i < size; i++){
                if (buffer[i] != '\0') strncat(new_buffer, &buffer[i], 1);
            } 
            buffer = new_buffer;
            size = strlen(buffer);
        }
        // seperate file into four parts and use four threads to zip each part
        pthread_t p[num_cpu];
        int chunk_size = size/num_cpu+1;
        for (int i = 0; i < num_cpu; i++){
            inputs *cur_arg = malloc(sizeof(inputs));
            cur_arg->buffer = buffer;
            cur_arg->start = i*chunk_size;
            cur_arg->end = (i+1)*chunk_size;
            pthread_create(&p[i], NULL, wzip, (void*)cur_arg);
            usleep(10000);
        }
        Zipped** zipped = malloc(sizeof(Zipped *) * num_cpu);
        // join all the threads and get files with zipped data
        for (int i = 0; i < num_cpu; i++){
            void* cur_zipped = malloc(sizeof(Zipped));    
            pthread_join(p[i], &cur_zipped);
            zipped[i] = (Zipped *)cur_zipped;
        }
        // fwrite zipped data to stdput with output files
        for(int i = 0; i < num_cpu; i++){
            for(int k = 0; k < chunk_size + 1; k++){
                if ((j != 1 || i != 0) && k == 0){
                    if (zipped[i]->character[k] == last_char)
                        zipped[i]->number[k] = zipped[i]->number[k] + last_num;
                    else{
                        //printf("l: %d%c\n", last_num, last_char);
                        fwrite(&last_num, sizeof(int), 1, stdout);
                        fwrite(&last_char, sizeof(char), 1, stdout);
                    }
                }
                if (k == chunk_size || zipped[i]->character[k+1] == '\0'){
                    last_num = zipped[i]->number[k];
                    last_char = zipped[i]->character[k];
                    break;
                }
                fwrite(&zipped[i]->number[k], sizeof(int), 1, stdout);
                fwrite(&zipped[i]->character[k], sizeof(char), 1, stdout);
            }
        }
        // close current file and free allocated struct
        free(zipped);     
        close(fd);
    }
    // fwrite the last data of the last file to stdout
    fwrite(&last_num, sizeof(int), 1, stdout);
    fwrite(&last_char, sizeof(char), 1, stdout);
    return 0;
}




        

