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
#include <limits.h>

char end_mark = '\0';

// struct for thread input
typedef struct{
    char *file_name;
    char *buffer;
    int start;
    int end;
}inputs;

void single_thread_zip(int argc, char *argv[]){
    char new = '\0';
    char old = '\0';
    int count = 1;
    for(int j = 1; j < argc; j++){
        struct stat s;
        int fd = open(argv[j], O_RDONLY);
        if (fd == -1) continue;
        fstat(fd, &s);
        size_t size = s.st_size;
        char *buffer = (char *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
        for (int i = 0; i < size; i++){
            if (buffer[i] == ' ' || buffer[i] == '\0') continue;
            old = new;
            new = buffer[i];
            if (old == 0) continue;
            if (old == new) count++;
            else{
                fwrite(&count, sizeof(int), 1, stdout);
                fwrite(&old, sizeof(char), 1, stdout);
                count = 1;
            }
        }  
        close(fd); 
    }
    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(&new, sizeof(char), 1, stdout);
}

void *wzip(void *arg) {
    // Getting input info from arg
    inputs *input = (inputs *)arg;
    char *file_name = input->file_name;
    char *buffer = input->buffer;
    int start = input->start;
    int end = input->end;
    int index = 0;
    // wzip given text
    char new = '\0';
    char old = '\0';
    int count = 1;
    FILE *fp = fopen(file_name, "w");
    for (int i = start; i < end; i++){
        if (buffer[i] == '\0' || buffer[i] == ' ') continue;
        old = new;
        new = buffer[i];
        if (old == 0) continue;
        if (old == new) count++;
        else{
            fprintf(fp, "%d,%c\n", count, old);
            index++;
            count = 1;
        }
    }
    fprintf(fp, "%d,%c\n", count, new);
    fclose(fp); 
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
    // check whether multi-thread is needed
    if (argc <= num_cpu+1){
        single_thread_zip(argc, argv);
        return 0;
    }
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
        close(fd);
        // deal with small file
        if (size < INT_MAX){
            for (int i = 0; i < size; i++){
                if (buffer[i] == ' ' || buffer[i] == '\0') continue;
                if (last_num == 0){ 
                    last_num = 1;
                    last_char = buffer[i];
                    continue;
                }
                if (last_char == buffer[i]){
                    last_num++;
                } else{
                    fwrite(&last_num, sizeof(int), 1, stdout);
                    fwrite(&last_char, sizeof(char), 1, stdout);
                    last_num = 1;
                    last_char = buffer[i];
                }
            }  
            continue;
        }
        // deal with sparse file
        char *new_buffer = malloc(sizeof(char)*size);
        if (size != strlen(buffer)){
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
            if (i == 0) cur_arg->file_name = "0.txt";
            if (i == 1) cur_arg->file_name = "1.txt";
            if (i == 2) cur_arg->file_name = "2.txt";
            if (i == 3) cur_arg->file_name = "3.txt";
            cur_arg->start = i*chunk_size;
            cur_arg->end = (i+1)*chunk_size;
            pthread_create(&p[i], NULL, wzip, (void*)cur_arg);
            //usleep(10000);
        }
        // join all the threads and get files with zipped data
        for (int i = 0; i < num_cpu; i++){    
            pthread_join(p[i], NULL);
        }
        // fwrite zipped data to stdput with output files 
        int current_num = 0;
        char current_char = '\0';
        for (int i = 0; i < num_cpu; i++){
            char *file_name;
            if (i == 0) file_name = "0.txt";
            if (i == 1) file_name = "1.txt";
            if (i == 2) file_name = "2.txt";
            if (i == 3) file_name = "3.txt";
            FILE *fp = fopen(file_name, "r");
            while (fscanf(fp, "%d,%c", &current_num, &current_char) != EOF) {
                if (last_num == 0) {
                    last_num = current_num;
                    last_char = current_char;
                } else {
                    if (current_char == last_char) {
                        last_num += current_num;
                    } else {
                        fwrite(&last_num, sizeof(int), 1, stdout);
                        fwrite(&last_char, sizeof(char), 1, stdout);
                        last_num = current_num;
                        last_char = current_char;
                    }
                }
            }
            fclose(fp);  
        }      
    }
    // fwrite the last data of the last file to stdout
    fwrite(&last_num, sizeof(int), 1, stdout);
    fwrite(&last_char, sizeof(char), 1, stdout);
    return 0;
}