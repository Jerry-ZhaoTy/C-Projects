///////////////////////////////////////////////////////////////////////////////
// Main File:        cache2Drows.c
// This File:        cache2Drows.c
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

int arr2D[3000][500]; // 3000 X 500 2-dimensional global array of integers

int main(){
    // Set the value of each element in the array in row_wise order
    for (int row = 0; row < 3000; row++)
        for (int col = 0; col < 500; col++)
            arr2D[row][col] = row + col;
    return 0;
}