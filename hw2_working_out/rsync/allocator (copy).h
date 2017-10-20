// allocator.h
#pragma once
#ifndef OS_CUSTOM_ALLOCATOR_H
#define OS_CUSTOM_ALLOCATOR_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// maximum block size in bytes: 2^20 = 1 MiB
#define MAX_EXP 20
#define MAX_SIZE 1<<MAX_EXP  //*TODO Find out what this means...DONE: left bit shift operator: 1<<20 = 1 MiB (see bit shift <<)

// block struct
struct block {
    size_t size;                          // size in bytes 
    bool free;                            // whether block is available for allocation
    void* data;                           // pointer to actual data
    struct block* next;                   // pointer to next block in list
    struct block* buddy;                  // pointer to buddy (block)
    struct block* merge_buddy[MAX_EXP];  //  pointer list for buddies of the merged blocks up the 'tree'
 };

// start of list (of pointers to blocks)
struct block* head;

// functions to be implemented in allocator.c
void* custom_malloc(size_t size);
void* custom_realloc(void* ptr, size_t size);
void  custom_free(void* ptr);

// debug functions
void print_block(struct block* b);
void print_list();

#endif
