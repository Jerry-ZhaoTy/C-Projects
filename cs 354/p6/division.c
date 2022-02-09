///////////////////////////////////////////////////////////////////////////////
// Main File:        divison.c
// This File:        divison.c
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

int counter = 0; // Global counter counts successful divisons

// Handler for zero division exception
void div_handler () {
    // Display error info
    printf("Error: a division by 0 operation was attempted.\n");
    // Display successful division counts, exit info and exit
    printf("Total number of operations completed successfully: %i\n", counter);
    printf("The program will be terminated.\n");
    exit(0);
}

// Handler for user interrrupt
void stop_handler(){
    // Display successful division counts, exit info and exit
    printf("\nTotal number of operations completed successfully: %i\n", counter);
    printf("The program will be terminated.\n");
    exit(0);
}

int main () {
    // Sigaction for zero division exception
    struct sigaction div;
    memset (&div, 0, sizeof(div)); 
    div.sa_flags = 0;
    div.sa_handler = div_handler;
    if (sigaction(SIGFPE, &div, NULL) != 0) {
        printf("Failed to bind handler.\n");
        exit(0);
    }
    // Sigaction for user interrupt
    struct sigaction stop;
    memset (&stop, 0, sizeof(stop));
    stop.sa_flags = 0;
    stop.sa_handler = stop_handler;
    if (sigaction(SIGINT, &stop, NULL) != 0) {
        printf("Failed to bind handler.\n");
        exit(0);
    }

    char str1[100]; // Variable for user's first input
    char str2[100]; // Variable for user's second input
    while(1){ //infinite loop until error or interrupt
        // Get user input
        printf("Enter first integer: ");
        scanf("%s", str1);
        printf("Enter second integer: ");
        scanf("%s", str2);
        // Change input to integers, non-integer values will be changed to 0
        int num1 = atoi(str1);
        int num2 = atoi(str2);
        // Calculate quotient and remainder, and display to user
        int result = num1 / num2;
        int remainder = num1 % num2;
        printf("%i / %i is %i with a remainder of %i\n", num1, num2, result, remainder);
        // Increment of successful divison's count
        counter++;
    }

    return 0;

}

