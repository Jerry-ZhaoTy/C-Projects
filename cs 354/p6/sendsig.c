///////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// This File:        sendsig.c
// Other Files:      none
// Semester:         CS 354 Spring 2021
// Instructor:       deppeler
// 
// Author:           Tianyi Zhao
// Email:            tzhao86@wisc.edu
// CS Login:         tzhao86
//
/////////////////////////// OTHER SOURCES OF HELP /////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide //////////////////////////////////

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

int main (int argc, char *argv[]) {
    // Check whether command line arguments number is correct
    if (argc != 3) {
        printf("Usage: sendsig <signal type> <pid>\n");
        exit(0);
    }
    // Parse pid
    pid_t pid = atoi(argv[2]);
    // Send signal corersponding to command line argument
    if (strcmp(argv[1], "i") == 0) {
        kill(pid, SIGINT);  
    }
    if (strcmp(argv[1], "u") == 0) {
        kill(pid, SIGUSR1);  
    }


    return 0;

}

