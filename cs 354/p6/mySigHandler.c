///////////////////////////////////////////////////////////////////////////////
// Main File:        mySigHandler.c
// This File:        mySigHandler.c
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

int timer = 3; // Global variable, number of seconds to arm the alarm
int counter = 0; // Global variable to count number of user signals handled

// Handler for alarm
void alarm_handler (){
    // Get current time
    time_t curtime;
    time(&curtime);
    // Print pid and current time
    printf("PID: %d CURRENT TIME: %s", getpid(), ctime(&curtime));
    // Re-arm the alarm
    alarm(timer);
}

// Handler for interrupt
void stop_handler (){
    // Print interrput-message and exit
    printf("\nSIGINT handled.\n");
    printf("SIGUSR1 was handled %i times. Exiting now.\n", counter);
    exit(0);
}

// Handler for user signal
void user1_handler (){
    // Increment of count of user signals handled
    counter++;
    // Display handling message
    printf("SIGUSR1 handled and counted!\n");
}

int main (){

    // Arm an alarm
    alarm(timer);
    // Sigaction for alarm
    struct sigaction act;
    memset (&act, 0, sizeof(act)); 
    act.sa_flags = 0;
    act.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &act, NULL) != 0) {
        printf("Failed to bind handler.\n");
        exit(0);
    }   
    // Sigaction for user signal 1
    struct sigaction user1;
    memset (&user1, 0, sizeof(user1));
    user1.sa_flags = 0;
    user1.sa_handler = user1_handler;
    if (sigaction(SIGUSR1, &user1, NULL) != 0) {
        printf("Failed to bind handler.\n");
        exit(0);
    }
    // Sigaction for interrupt
    struct sigaction stop;
    memset (&stop, 0, sizeof(stop));
    stop.sa_flags = 0;
    stop.sa_handler = stop_handler;
    if (sigaction(SIGINT, &stop, NULL) != 0) {
        printf("Failed to bind handler.\n");
        exit(0);
    }
    // Print basic info and exit info
    printf("PID and time print every 3 seconds.\n");
    printf("Type Ctrl-C to end the program.\n");
    // The infinite loop
    while (1);

    return 0;

}

