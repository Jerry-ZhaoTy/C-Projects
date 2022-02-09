#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Structure storing user input data
struct Data{
    int key;
    char *value;
};
char delim[] = ",";

int main(int argc, char *argv[]) {

    // Check if there is a command line argument
    if (argc == 1){
        return 0;
    }

    // Open an existing database file to retrieve data
    FILE *fp = fopen("database.txt", "r");
    int curIndex = 0;
    struct Data database[100000];  
    if (fp != NULL) {
        // Get all data stored in database
        char *curData;
        size_t size = 100000;
        curData = (char*) malloc (size);
        while (getline(&curData, &size, fp)!= -1){
	    int k = atoi(strsep(&curData, delim));
            char *v = strsep(&curData, delim);
            struct Data curData = {k, v};
            database[curIndex] = curData;
            curIndex++;
        }
        // Close the file
        if (fclose(fp) != 0) {
            printf("Error while closing the file.\n");
            exit(1);
        }
    }

    int change = 0;
    // Operate based on user's commands
    for (int i = 1; i < argc; i++){
        // Get info from argument
        char *curArg = argv[i];
        char *curCommand = strsep(&curArg, delim);
        // Operation for 'put' command
        if (*curCommand == 'p'){
            char *key = strsep(&curArg, delim);
            if (key == NULL) {
                printf("bad command\n");
                continue;
	    }
            int k = atoi(key);
            if (k == 0 && *key != '0') {
                printf("bad command\n");
                continue;
	    }
            char *v = strsep(&curArg, delim);
            if (v == NULL) {
                printf("bad command\n");
                continue;
	    } 
            char *check = strsep(&curArg, delim);
            if (check != NULL) {
                printf("bad command\n");
                continue;
	    } 
            int found = 0;
            // Check if given key exists
            for (int i = 0; i < curIndex; i++){
                // If it exists, then update
                if (database[i].key == k){
                    struct Data curData = {k, v};
                    database[i] = curData;
                    found++;
                    change++;
                    break;
                }
            }
            // Otherwise, insert a new pair
            if (found == 0){
                struct Data curData = {k, v};
                database[curIndex] = curData;
                curIndex++;
                change++;
            }
        }
        // Operation for 'get' command
        else if (*curCommand == 'g'){
            char *key = strsep(&curArg, delim);
            if (key == NULL) {
                printf("bad command\n");
                continue;
	    }
            int k = atoi(key);
            if (k == 0 && *key != '0') {
                printf("bad command\n");
                continue;
	    }
            char *check = strsep(&curArg, delim);
            if (check != NULL) {
                printf("bad command\n");
                continue;
	    } 
            int found = 0;
            for (int i = 0; i < curIndex; i++){
                if (database[i].key == k){
                    printf("%d,%s\n", database[i].key, database[i].value);
                    found++;
                    break;
                }
            }
            if (found == 0) printf("%d not found\n", k);
        }
        // Operation for 'delete' command
        else if (*curCommand == 'd'){ 
            char *key = strsep(&curArg, delim);
            if (key == NULL) {
                printf("bad command\n");
                continue;
	    }
            int k = atoi(key);
            if (k == 0 && *key != '0') {
                printf("bad command\n");
                continue;
	    }
            char *check = strsep(&curArg, delim);
            if (check != NULL) {
                printf("bad command\n");
                continue;
	    } 
            int found = 0;
            for (int i = 0; i < curIndex; i++){
                if (database[i].key == k){
                    for(int j = i; j < curIndex; j++){
                        database[j] = database[j+1];
                    }
                    curIndex--;
                    found++;
                    change++;
                    break;
                }
            }
            if (found == 0) printf("%d not found\n", k);
        }
        // Operation for 'clear' command
        else if (*curCommand == 'c'){
            char *check = strsep(&curArg, delim);
            if (check != NULL) {
                printf("bad command\n");
                continue;
	    } 
            curIndex = 0;
            fclose(fopen("database.txt", "w"));
        }
        // Operation for 'all' command
        else if (*curCommand == 'a'){
            char *check = strsep(&curArg, delim);
            if (check != NULL) {
                printf("bad command\n");
                continue;
	    } 
            for (int i = 0; i < curIndex; i++){
                printf("%d,%s\n", database[i].key, database[i].value);
            }
        }
        // Operation for bad command
        else{
            printf("bad command\n");
        }
    }

    if (change != 0){
        // Write updated data back to database.txt
        fp = fopen("database.txt", "w");
        for (int i = 0; i < curIndex; i++){
	    fprintf(fp, "%d,%s,\n", database[i].key, database[i].value);
        }
        // Close the file
        if (fclose(fp) != 0) {
            printf("Error while closing the file.\n");
            exit(1);
        }
    }

}