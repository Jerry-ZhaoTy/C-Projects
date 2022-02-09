///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2021 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission Fall 2020, CS354-deppeler
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        myHeap.c
// This File:        myHeap.c
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
// DEB'S PARTIAL SOLUTION FOR SPRING 2021 DO NOT SHARE
 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "myHeap.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct blockHeader {

    int size_status;
    /*
     * Size of the block is always a multiple of 8.
     * Size is stored in all block headers and in free block footers.
     *
     * Status is stored only in headers using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     *
     * End Mark:
     *  The end of the available memory is indicated using a size_status of 1.
     *
     * Examples:
     *
     * 1. Allocated block of size 24 bytes:
     *    Allocated Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 25
     *      If the previous block is allocated p-bit=1 size_status would be 27
     *
     * 2. Free block of size 24 bytes:
     *    Free Block Header:
     *      If the previous block is free      p-bit=0 size_status would be 24
     *      If the previous block is allocated p-bit=1 size_status would be 26
     *    Free Block Footer:
     *      size_status should be 24
     */
} blockHeader;

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */
blockHeader *heapStart = NULL;

/* Size of heap allocation padded to round to nearest page size.
 */
int allocsize;

/*
 * Additional global variables may be added as needed below
 */

 
/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block (payload) on success.
 * Returns NULL on failure.
 *
 * This function must:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8
 *   and possibly adding padding as a result.
 *
 * - Use BEST-FIT PLACEMENT POLICY to chose a free block
 *
 * - If the BEST-FIT block that is found is exact size match
 *   - 1. Update all heap blocks as needed for any affected blocks
 *   - 2. Return the address of the allocated block payload
 *
 * - If the BEST-FIT block that is found is large enough to split
 *   - 1. SPLIT the free block into two valid heap blocks:
 *         1. an allocated block
 *         2. a free block
 *         NOTE: both blocks must meet heap block requirements
 *       - Update all heap block header(s) and footer(s)
 *              as needed for any affected blocks.
 *   - 2. Return the address of the allocated block payload
 *
 * - If a BEST-FIT block found is NOT found, return NULL
 *   Return NULL unable to find and allocate block for desired size
 *
 * Note: payload address that is returned is NOT the address of the
 *       block header.  It is the address of the start of the
 *       available memory for the requesterr.
 *
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* myAlloc(int size) {
    
    // Check whetehr input size is positive
    if (size <= 0)
        return NULL;
    
    // Add bytes for header
    int adjusted_size = size + 4;
    // Rounding up to a multiple of 8
    if (adjusted_size % 8 != 0)
        // Add padding if not a multiple of 8
        adjusted_size = adjusted_size + (8 - adjusted_size % 8);
    
    // Use BEST-FIT PLACEMENT POLICY to chose a free block
    blockHeader *header = heapStart; // Header of first block
    blockHeader *address = NULL; // Address of the allocated block payload
    int difference = -1; // variable to help find best-fit block
    // Visit all blocks until we meet the end mark
    while(header -> size_status != 1){
        
        // Get size status of current visited block
        int status = header -> size_status;
        // Calculate size of current block
        int cur_size = status - status % 8;
        // Check if current block is a free block
        if (status % 8 == 0 || status % 8 == 2){
            // If perfect fit free block is found, break
            if (adjusted_size == cur_size){
                address = header;
                break;
            }
            else if (adjusted_size < cur_size) {
                // Change the best_fit block if block with closer size met
                if(difference == -1 || cur_size - adjusted_size < difference) {
                    difference = cur_size - adjusted_size;
                    address = header;
                }
            }
        }
        // Move to header of next block base on current block size
        header = (blockHeader*) ((void*)header + cur_size);
    }
    
    // Check if a big enough free block has been found
    if (address == NULL)
        return NULL;
    
    // Get status and size of best_fit free block
    int final_status = address -> size_status;
    int final_size = final_status - final_status % 8;
    // Check whether the block found is large enough to split
    if (final_size - adjusted_size < 8){
        // Update header to show allocated
        address -> size_status += 1;

        // Check if selected block is the block before end mark
        blockHeader *nextHeader = (blockHeader*) ((void*)address + final_size);
        if (nextHeader -> size_status != 1){
	    // Update header of next block to show prev allocated if not end mark
            nextHeader -> size_status += 2;
        }

        // add four to skip header and return best_fit block address
        return (void*)address + 4;
    }
    else {
	// Calculate size of splitted free block
        int split_size = final_size - adjusted_size;

        // Update header to show allocated
        address -> size_status = adjusted_size + final_status % 8 + 1;

	// Create header of splitted block to show prev allocated
        blockHeader *splitHeader 
            = (blockHeader*) ((void*)address + adjusted_size);
        splitHeader -> size_status = split_size + 2;
        // Create footer of splitted block to indicate free block size
        blockHeader *footer 
            = (blockHeader*) ((void*)splitHeader + split_size - 4);
        footer -> size_status = split_size;

	// add four to skip header and return best_fit block address
        return (void*)address + 4;
    }

}
 
/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - Update header(s) and footer as needed.
 */
int myFree(void *ptr) {
    
    // Check whether input ptr is valid
    blockHeader *endBlock = (blockHeader*) ((void*) heapStart + allocsize);
    if (ptr == NULL || (int) ptr % 8 != 0 || ptr > (void*) endBlock)
    	return -1;
    
    // Get block header
    blockHeader *header = (blockHeader*) ((void*) ptr - 4);

    // Check if given ptr is already freed
    if (header -> size_status % 8 == 0 || header -> size_status % 8 == 2)
	return -1;

    // Set header of given ptr block to indicate a free block
    header -> size_status -= 1;

    // Set header of next block to indicate prev free except for end mark
    int status = header -> size_status;
    int size = status - status % 8;
    blockHeader *nextHeader = (blockHeader*) ((void*) ptr - 4 + size);
    // Check if next block is the end mark
    if (nextHeader -> size_status != 1)
    	nextHeader -> size_status -= 2;

    return 0;
}

/*
 * Function for traversing heap block list and coalescing all adjacent
 * free blocks.
 *
 * This function is used for delayed coalescing.
 * Updated header size_status and footer size_status as needed.
 */
int coalesce() {
     
    // Set a variable to indicate whether adjacent free blocks coalesced
    int coalesce = 0;
    
    // Travers the heap from start to end
    blockHeader *header = heapStart;
    while(header -> size_status != 1){

        // Get status of current visited block
	int cur_size_status = header -> size_status;
        int cur_size = cur_size_status - cur_size_status % 8;

        // Go to next block if current block is allocated
        if (cur_size_status % 8 == 1 || cur_size_status % 8 == 3) {
            header = (blockHeader*) ((void*) header + cur_size);
            continue;
        }
        
        // Get status of next block if current block is a free block
        blockHeader *nextHeader = (blockHeader*) ((void*) header + cur_size);
        int next_size_status = nextHeader -> size_status;
        int next_size = next_size_status - next_size_status % 8;

        // End the loop if end mark reached
        if (next_size_status == 1) break;

        // Check if next block is a free block
        if (next_size_status % 8 == 0){
            // Calculate new size and size_status with the adjacent free blocks
            int new_size = cur_size + next_size;
            int new_status = new_size + 2;
            // Update size_status of coalesced new free block
            header -> size_status = new_status;
            // Update footer of coalesced new free block
            blockHeader *footer 
                = (blockHeader*) ((void*) header + new_size - 4);
            footer -> size_status = new_size;
            coalesce = 1; // update coalesce status
        }
        // Move to next block if no coalesce taken place
        else header = (blockHeader*) ((void*) header + cur_size);
    	
    }

    return coalesce;
}

 
/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int myInit(int sizeOfRegion) {
 
    static int allocated_once = 0; //prevent multiple myInit calls
 
    int pagesize;   // page size
    int padsize;    // size of padding when heap size not a multi of page size
    void* mmap_ptr; // pointer to memory mapped area
    int fd;

    blockHeader* endMark;
  
    if (0 != allocated_once) {
        fprintf(stderr,
        "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }

    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    allocsize = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    allocsize -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heapStart = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    endMark = (blockHeader*)((void*)heapStart + allocsize);
    endMark->size_status = 1;

    // Set size in header
    heapStart->size_status = allocsize;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heapStart->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heapStart + allocsize - 4);
    footer->size_status = allocsize;
  
    return 0;
}
                  
/*
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void dispMem() {
 
    int counter;
    char status[6];
    char p_status[6];
    char *t_begin = NULL;
    char *t_end   = NULL;
    int t_size;

    blockHeader *current = heapStart;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used   = -1;

    fprintf(stdout,
    "*********************************** Block List **********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout,
    "---------------------------------------------------------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "alloc");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "FREE ");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "alloc");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "FREE ");
        }

        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%4i\n", counter, status,
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout,
    "---------------------------------------------------------------------------------\n");
    fprintf(stdout,
    "*********************************************************************************\n");
    fprintf(stdout, "Total used size = %4d\n", used_size);
    fprintf(stdout, "Total free size = %4d\n", free_size);
    fprintf(stdout, "Total size      = %4d\n", used_size + free_size);
    fprintf(stdout,
    "*********************************************************************************\n");
    fflush(stdout);

    return;
}


// end of myHeap.c (sp 2021)  
