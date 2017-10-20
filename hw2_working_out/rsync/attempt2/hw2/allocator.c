// allocator.c
#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>



/** Allocate a block of at least the requested size **/
void* custom_malloc(size_t size) {
    // TODO
  // pthread_mutex_t global_malloc_lock;   I THINK EVERYONE WILL FORGET TO DO THIS!!
        
       void* p= sbrk(1048576);
        printf("after   %p\n",p);
        head=(struct block*) p;
        printf("after   %p\n",head);
        head->free=true;
       // head->size=1048575;
      //  head
       // head=sbrk(size);
        // head->size = MAX_SIZE;
        // head->next=NULL;
        // head->buddy=NULL;
        // for(int k=0;k<MAX_EXP;k++){
        //     head->merge_buddy[k]=NULL; //?? // may need to be done in a forloop.Why is this not done in struct definition?
        // }
    


    //return NULL;
}

/** Mark a data block as free and merge free buddy blocks **/
void custom_free(void* ptr) {
    // TODO
}

/** Change the memory allocation of the data to have at least the requested size **/
void* custom_realloc(void* ptr, size_t size) {
    // TODO
    return NULL;
}

/*------------------------------------*\
|            DEBUG FUNCTIONS           |
\*------------------------------------*/

/** Prints the metadata of a block **/
void print_block(struct block* b) {
    if(!b) {
        printf("NULL block\n");
    }
    else {
        int i = 0;
        printf("Strt = %p\n",b);
        printf("Size = %lu\n",b->size);
        printf("Free = %s\n",(b->free)?"true":"false");
        printf("Data = %p\n",b->data);
        printf("Next = %p\n",b->next);
        printf("Buddy = %p\n",b->buddy);    
        printf("Merge Buddies = "); 
        while(b->merge_buddy[i] && i < MAX_EXP) {
            printf("%p, ",b->merge_buddy[i]);
            i++;
        }
        printf("\n\n");
    }
}

/** Prints the metadata of all blocks **/
void print_list() {
    struct block* curr = head;
    printf("--HEAP--\n");
    if(!head) printf("EMPTY\n");
    while(curr) {
        print_block(curr);
        curr = curr->next;
    }
    printf("--END--\n");
}
