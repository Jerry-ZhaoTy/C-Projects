#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    
    if (argc < 2) {
        printf("pzip: file1 [file2 ...]\n");
        exit(1);
    }

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

    return 0;
}


