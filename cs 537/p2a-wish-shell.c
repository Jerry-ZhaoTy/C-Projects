#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

char error_message[30] = "An error has occurred\n";    

void shellHelper(char *curCmd, char *paths[], int *path_num){

    // Remove new line character
    char *parse_cmd;
    while ((parse_cmd = strsep(&curCmd, "\n")) != NULL){
        if (strlen(parse_cmd) != 0){
            curCmd = parse_cmd;
            break;
        } 
    }

    // Ignore empty input
    if (curCmd == NULL) return;

    // Get user command and redirection file
    char *c = strsep(&curCmd, ">");
    char *r = strsep(&curCmd, ">");

    // Remove starting spaces and check if there is an input
    curCmd = (char*) malloc (100);
    strcpy(curCmd, c);
    curCmd = strsep(&curCmd, "\n");
    c = strsep(&curCmd, " ");
    while (c != NULL && strlen(c) == 0) c = strsep(&curCmd, " ");
    if (c == NULL) {
        if (r != NULL) write(STDERR_FILENO, error_message, strlen(error_message)); 
        return;
    }

    // Check if redirection file is valid
    char * file_name = NULL;
    if (r != NULL){
        char *rf = (char*) malloc (100);
        strcpy(rf, r);
        rf = strsep(&rf, "\n");
        r = strsep(&rf, " ");
        while(r != NULL && strlen(r) == 0) r = strsep(&rf, " ");   
        if (r != NULL){
            file_name = r;
            if (strsep(&rf, " ") != NULL) {
                write(STDERR_FILENO, error_message, strlen(error_message)); 
                return;
            }
        }
        else{
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            return;
        }
    }

    // Check if given command is built-in command
    if (strcmp(c, "exit") == 0) {
        char *argv = strsep(&curCmd, " ");
        while (argv != NULL && strlen(argv) == 0) argv = strsep(&curCmd, " ");
        if (argv == NULL) exit(0);
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        return;
    }
    if (strcmp(c, "cd") == 0){
        // Check if there is a valid input
        char *argv1 = strsep(&curCmd, " ");
        while (argv1 != NULL && strlen(argv1) == 0) argv1 = strsep(&curCmd, " ");
        if (argv1 == NULL){
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            return;
        }
        // Check if only exactly one input is given
        char *argv2 = strsep(&curCmd, " ");
        while (argv2 != NULL && strlen(argv2) == 0) argv2 = strsep(&curCmd, " ");
        if (argv2 != NULL){ 
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        chdir(argv1);
        return;
    } 
    if (strcmp(c, "path") == 0){
        char *path;  
        *path_num = 0;      
        while ((path = strsep(&curCmd, " ")) != NULL){
            if (strlen(path) == 0) continue;
            paths[*path_num] = path;
            (*path_num)++;
        }
        return;
    }
    if (strcmp(c, "loop") == 0){
        // Check whether loop-times is given
        char *t = strsep(&curCmd, " ");
        if (t == NULL){
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            return;
        }
        int time = atoi(t);

        // Set command line arguments
        char *cmd_argv[10];
        char *argv;
        int index = 0;
        while ((argv = strsep(&curCmd, " ")) != NULL){
            if (strlen(argv) == 0) continue;
            cmd_argv[index] = argv;
            index++;
        }
        cmd_argv[index] = NULL;

        // Set path for loop command
        char *path = (char*) malloc (100);
        strcpy(path, paths[0]);
        path = strsep(&path, "\n");
        strcat(path, "/");
        strcat(path, cmd_argv[0]);
        int loop_var_cmd = 0;
        if (strcmp(cmd_argv[0], "$loop") == 0) loop_var_cmd = 1;

        int fail = 1;
        // Loop with given time
        for (int i = 1; i <= time; i++){
            // replacing loop variable with number
            char loop_var = i + '0';
            char last_loop_var = i - 1 + '0';
            char lastLoopVar[2] = {last_loop_var, '\0'};
            index = 1;
            while(cmd_argv[index] != NULL){
                if (strcmp(cmd_argv[index], "$loop") == 0 || strcmp(cmd_argv[index], lastLoopVar) == 0){
                    char loopVar[2] = {loop_var, '\0'};
                    cmd_argv[index] = loopVar;
                }
                index++;
            }
            
            if (loop_var_cmd == 1){
                cmd_argv[0] = &loop_var;
                strcpy(path, paths[0]);
                path = strsep(&path, "\n");
                strcat(path, "/");
                strncat(path, &loop_var, 1);
            }

            if (access(path, X_OK) == 0){
                fail = 0;
                int rc = fork();
                if (rc == 0) execv(path, cmd_argv);
                else if (rc > 0) (void)wait(NULL);
                else exit(1);                
            }
        }  
        if (fail == 1) write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    } // end of loop

    // Dealing with none built-in command
    // Set up command line argument
    char *cmd_argv[10];
    char *argv;
    int index = 1;
    while ((argv = strsep(&curCmd, " ")) != NULL){
        if (strlen(argv) == 0) continue;
        cmd_argv[index] = argv;
        index++;
    }
    cmd_argv[index] = NULL;

    // Find the correct path the program for user command resides
    int fail = 1;
    for (int i = 0; i < *path_num; i++){
        if (fail == 0) break;
        char *path = (char*) malloc (100);
        strcpy(path, paths[i]);
        path = strsep(&path, "\n");
        strcat(path, "/");
        strcat(path, c);
        cmd_argv[0] = c;
        if (access(path, X_OK) == 0){
            fail = 0;
            int rc = fork();
            if (rc == 0) {
                if (file_name != NULL){
                    (void) close(STDOUT_FILENO);
                    open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }
                execv(path, cmd_argv);
            }
            else if (rc > 0) (void)wait(NULL);
            else exit(1);
        } 
    }
    if (fail == 1) write(STDERR_FILENO, error_message, strlen(error_message));
    return;
}

int main(int argc, char *argv[]) {

    // Check if input format is correct
    if (argc > 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    // Set up default path
    char *paths[100];
    int num_1 = 1;
    int *path_num = &num_1;
    paths[0] = strdup("/bin");

    // Check whether the user chose interactive mode or not	
    if (argc == 2){
        // Open the file and check if it opened successfully.
        FILE *fp = fopen(*(argv + 1), "r");
        if (fp == NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        char *curCmd;
        size_t size = 100;
        curCmd = (char*) malloc (size);
        while (getline(&curCmd, &size, fp) != -1){
            shellHelper(curCmd, paths, path_num);
        }
        return 0;
    }
    
    // Enter interactive mode
    while(1){
        printf("wish> ");
        char *curCmd;
        size_t size = 100;
        curCmd = (char*) malloc (size);
        getline(&curCmd, &size, stdin);
        shellHelper(curCmd, paths, path_num);
    }  

    return 0;
}