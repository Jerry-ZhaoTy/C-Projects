///////////////////////////////////////////////////////////////////////////////
// Main File:        cache1D.c
// This File:        cache1D.c
// Other Files:      None
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

int a[100000]; // Declare a global array of integers of size 100,000

int main(){
    // Set the value of each element in the array to its index in a loop
    for (int i = 0; i < 100000; i++)
        a[i] = i;
    return 0;
}