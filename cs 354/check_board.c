///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        check_board.c
// This File:        check_board.c
// Other Files:      none
// Semester:         CS 354 Spring 2021
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
     
// commas ',' are a common delimiter character for data strings
char *DELIM = ",";
     
/* COMPLETED (DO NOT EDIT):
 * Read the first line of input file to get the size of that board.
 *
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 */
void get_board_size(FILE *fptr, int *size) {
    char *line1 = NULL;
    size_t len = 0;
    if ( getline(&line1, &len, fptr) == -1 ) {
        printf("Error reading the input file.\n");
        exit(1);
    }

    char *size_chars = NULL;
    size_chars = strtok(line1, DELIM);
    *size = atoi(size_chars);
}



/*
 * Returns 1 if and only if the board is in a valid Sudoku board state.
 * Otherwise returns 0.
 *
 * A valid row or column contains only blanks or the digits 1-size,
 * with no duplicate digits, where size is the value 1 to 9.
 *
 * Note: p2A requires only that each row and each column are valid.
 *
 * board: heap allocated 2D array of integers
 * size:  number of rows and columns in the board
 */
int valid_board(int **board, int size) {
        
    // Visit each row of the given board
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            for (int k = j+1; k < size; k++){
                
                // Skip the check for zeros
                if(*(*(board + i) + j) == 0)
                    continue;
                
                // Check whether there are duplicate digits in a row
                if(*(*(board + i) + j) == *(*(board + i) + k))
                    // Found duplicate digits in a row, invalid
                    return 0;
            }
        }
    }
    
    // Visit each column of the given board
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            for (int k = j+1; k < size; k++){
                
                // Skip the check for zeros
                if(*(*(board + j) + i) == 0)
                    continue;
                
                // Check whether there are duplicate digits in a column
                if(*(*(board + j) + i) == *(*(board + k) + i))
                    // Found duplicate digits in a column, invalid
                    return 0;
            }
        }
    }

    // No duplicate digits are found on same rows or columns, valid
    return 1;
}
  
 
   
/*
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 *
 * argc: CLA count
 * argv: CLA value
 */
int main( int argc, char *argv[] ) {
    
    // Check if number of command-line arguments is correct.
    if (argc != 2){
        printf("Usage: ./check_board <input_filename>\n");
        exit(1);
    }
    // Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
        printf("Can't open file for reading.\n");
        exit(1);
    }

    // Declare local variables.
    int size;

    // Call get_board_size to read first line of file as the board size.
    int *board_size = &size;
    get_board_size(fp, board_size);
    
    // Dynamically allocate a 2D array for given board size.
    int **board;
    board = malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++){
        *(board + i) = malloc(sizeof(int) * size);
    }
    *board = malloc(sizeof(int) * size);
    
    // Read the rest of the file line by line.
    // Tokenize each line wrt the delimiter character
    // and store the values in your 2D array.
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < size; i++) {

        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading line %i of the file.\n", i+2);
            exit(1);
        }

        token = strtok(line, DELIM);
        
        for (int j = 0; j < size; j++) {
            
            // initialization of the 2D array.
            *(*(board + i) + j) = atoi(token);
            token = strtok(NULL, DELIM);
        }
    }

    // Call the function valid_board to check whether the board is valid
    int check_valid = valid_board(board, size);
    
    // Output depending on the function's return value.
    if (check_valid == 1) printf("valid\n");
    else printf("invalid\n");
    
    // Free all dynamically allocated memory.
    for (int i = 0; i < size; i++){
        free(*(board + i));
    }
    free(board);
    
    //Close the file.
    if (fclose(fp) != 0) {
        printf("Error while closing the file.\n");
        exit(1);
    }
     
    return 0;
}






// s21
