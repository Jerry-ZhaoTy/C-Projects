///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2021, Deb Deppeler
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        myMagicSquare.c
// This File:        myMagicSquare.c
// Other Files:      none
// Semester:         CS 354 Spring 2021
// Instructor:       deppeler
//
// Author:           Tianyi Zhao
// Email:            tzhao86@wisc.edu
// CS Login:         tzhao
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
    int size;           // dimension of the square
    int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/*
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
int getSize() {
    
    // Declare var to store size
    int size;
    
    // Prompt for size and read user's input
    printf("Enter magic square's size (odd integer >=3)\n");
    scanf("%d",&size);
    
    // Check whether the input size is valid
    if (size < 3){
        printf("Magic square size must be >= 3.\n");
        exit(1);
    }
    if (size % 2 == 0){
        printf("Magic square size must be odd.\n");
        exit(1);
    }
        
    // Return valid size
    return size;
}
   
/*
 * Makes a magic square of size n using the alternate
 * Siamese magic square algorithm from assignment and
 * returns a pointer to the completed MagicSquare struct.
 *
 * n the number of rows and columns
 */
MagicSquare *generateMagicSquare(int n) {
    
    int size = n;
    
    // Initialize and allocate heap memory for a 2D square
    int **square;
    square = malloc(sizeof(int *) * size);
    
    // Check whether allocation fails
    if (square == NULL){
        printf("Allocation of memory fails");
        exit(1);
    }
    
    // Allocate memory for each row
    for (int i = 0; i < size; i++){
        
        *(square + i) = malloc(sizeof(int) * size);
        
        // Check whether allocation fails
        if(*(square + i) == NULL){
            printf("Allocation of memory fails");
            exit(1);
        }
    }
    
    // Explicitly initialize every element to zero
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            *(*(square + i) + j) = 0;
        }
    }
    
    // Generate the magic square with Siaese method
    int i = 1;
    int startRow = 0;
    int startCol = size/2;
    
    // The middle index of the first row is set to 1
    *(*(square + startRow) + startCol) = i;
    
    while (i < size * size){
        
        i++; // Value to be set in each iteration
        
        // Update of row index for next value
        if (startCol + 1 < size)
            startCol++;
        else
            startCol = 0;

        // Update of col index for next value
        if (startRow - 1 >= 0)
            startRow--;
        else
            startRow = size - 1;
        
        // Check whether updated position is used
        if( *(*(square + startRow) + startCol) != 0){
            
            // Update the position again if used
            if(startRow - 1 > 0)
                startRow--;
            else
                startRow = size - 1;
            if(startCol + 2 < size)
                startCol += 2;
            else
                startCol = 0;
        }
        
        // Set the updated position to i
        *(*(square + startRow) + startCol) = i;
        
    }
    
    // Create a pointer to a MagicSquare struct
    MagicSquare *magicSquare = malloc(sizeof(MagicSquare));
    
    // Check whether allocation fails
    if(magicSquare == NULL){
        printf("Allocation of memory fails");
        exit(1);
    }
    
    // Assign size and the square to the struct and return
    magicSquare -> size = size;
    magicSquare -> magic_square = square;
    
    return magicSquare;
}

/*
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * magic_square the magic square to write to a file
 * filename the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
    
    // Open given file and check whether it opens successfully
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Can't open file.\n");
        exit(1);
    }
    
    // Get the size and square from given MagicSquare
    int size = magic_square -> size;
    int **magicSquare = magic_square->magic_square;
    
    // The first line indicates the size of the sqaure
    fprintf(fp, "%i", size);
    fprintf(fp, "%s", "\n");
    
    // Writes the square into the file
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            
            fprintf(fp, "%i", *(*(magicSquare + i) + j));
            // No comma after last element in a row
            if(j != size - 1) fprintf(fp, "%s", ",");
        }
        
        // Change line for the next row
        fprintf(fp, "%s", "\n");
    }
    
    // Close the file
    if (fclose(fp) != 0) {
        printf("Error while closing the file.\n");
        exit(1);
    }
}

/*
 * Generates a magic square of the user specified size and
 * output the quare to the output filename
 */
int main(int argc, char *argv[]) {

    // Check input arguments to get output filename
    if (argc != 2){
        printf("Usage: ./myMagicSquare <output_filename>\n");
        exit(1);
    }
    char *fileName = *(argv + 1);
    
    // Get magin square's size from user
    int size = getSize();
    
    
    // Generate the magic square
    MagicSquare *magicSquare = generateMagicSquare(size);
    
    // Output the magic square
    fileOutputMagicSquare(magicSquare, fileName);

    // Free allocated heap memory
    for (int i = 0; i < size; i++){
        free(*(magicSquare -> magic_square + i));
    }
    free(magicSquare -> magic_square);
    free(magicSquare);
    
    return 0;
}






                                                         
//        s21        myMagicSquare.c
