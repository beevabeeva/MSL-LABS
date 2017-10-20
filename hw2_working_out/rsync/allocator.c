// Resources: 
//        Brush up on void pointers:                 http://www.c4learn.com/c-programming/c-void-pointers/ 
//        Pointer arithmetic concepts:               https://www.cs.umd.edu/class/sum2003/cmsc311/Notes/BitOp/pointer.html
//        Mandatory reading of relevant man pages:   http://man7.org/linux/man-pages/man3/malloc.3.html

// allocator.c
#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

/// Helper function go here, because compiler we're using has a thing for forward declaration. 
// TODO Read up more about this...
// DONE: OK, it turns out that when there is no int type returning function, as with 'int main', 
// non-forward declaration is an issue. Kind of makes sense actually.
// I think I remember Richard telling me this in first year, all those years ago...
// TODO Put this in my COMSCI Book for future use.


/*------------------------------------*\
|           HELPER FUNCTIONS           |
\*------------------------------------*/

void block_split(struct block* b){
    struct block* new;
    for(int g=0;g<MAX_EXP;g++){
        b->merge_buddy[g]=NULL; //?? // may need to be done in a forloop.Why is this not done in struct definition?
    }       
    b->size=0.5*(b->size);  // Halve the original block's size.
    new->size= b->size;     // Set new block's size to half the original block's size.
    new->next=b->next;      // Insert new block into linked-list, between original and original->next.
    b->next=new;            // Maintain order & integrity of linked-list.
    b->buddy=new;           // Remember b's new buddy.
    new->buddy=b;           // Remember new's new buddy.
    new=b+b->size;          // * I THINK THIS IS HOW(?)*  Ptr to new block. This must be start of block b + size of block b.
    new->data=new+1;        // Ptr arithmetic to start data ptr after meta data of block.

    int i,j=0;
    // Iterate through b's buddy_array till you're at first NULL ptr. This is finds position in array.
    while(b->merge_buddy[i]!=NULL && i<MAX_EXP){
        i++ ;
    }
    // Iterate through new's buddy_array till you're at first NULL ptr. This is finds position in array.
    while(new->merge_buddy[j]!=NULL && j<MAX_EXP){
        j++ ;
    }
    // Add buddies to respective arrays:
    b->merge_buddy[i]=new;
    new->merge_buddy[j]=b;
}



/**round up size to nearest power of 2: **/
size_t get_nearest_pow_2(size_t size){
    double x;
    size_t p;       //NBNB MIGHT CAUSE PROBLEMS
    x=ceil(log2(size));
    p=pow(x,2);

    return p;
}



struct block* find_best_fit(size_t size){  //Function returns pointer to a block or NULL...Check if this is how to do it
//Iterate through block-list to smallest block of size  at least 2^k.
    struct block* cblock = head;
    struct block* smallest=head; //TODO fix this init
    while(cblock->next != NULL){    //while not at end...
        if (cblock->free){ //if curr block is free
            if((cblock->size <= smallest->size) && (cblock->size >=size)){ //if curr block is smaller than 'smallest' and still within acceptable size.
                smallest=cblock;            // find smallest block s.t conditions hold.
                cblock=cblock->next;        // next step
            }else{cblock=cblock->next;} // TODO:fix this if mess
        }else{cblock=cblock->next;}
    }
    if(smallest!=NULL) {
        return(smallest);           //Make sure smallest returns pointer to smallest block
    }else{
        return(NULL);               //TODO: MAKE SURE THIS IS CORRET
    }
}
  


// Test if splittable. 
bool is_splittable(struct block* b, size_t required){ //takes in: pointer to candidate block , required size (*NB:includes metadata and is rounded, at this point)
    size_t hsize = 0.5*(b->size);
    if( hsize>= required ){     // If half of block can hold required, it can hold meta, thus we can split it.
        return(true);
    }
    return(false);
}



/*------------------------------------*\
|            'MAIN' FUNCTIONS          |
\*------------------------------------*/

/** Allocate a block of at least the requested size **/
void* custom_malloc(size_t size) {
    // TODO
    if(size==0){            //if invalid size request. **TODO:check for other cases.
        return (NULL);
    }    
    //Check if this is the first malloc request for process:
    if (head == NULL){        // if head is uninitialised...then it's the first call of malloc.
        //head=sbrk(0);      // Init head 
        //*TODO:Note pointer arithmetic (might need ref/dref fixing) 
        head=sbrk(MAX_SIZE);      // If it is, extend heap by max, creating the OG big block. sbrk returns pointer to start of heap.
               // print_block(head);
        head->size = MAX_SIZE;
        head->next=NULL;
        head->buddy=NULL;   //Necessary?
        for(int k=0;k<MAX_EXP;k++){
            head->merge_buddy[k]=NULL; //?? // may need to be done in a forloop.Why is this not done in struct definition?
        }
        //print_block(head);

     }
    
    // round up requested size + metadata, to value of form 2^k:
    size_t rsize=get_nearest_pow_2(size+sizeof(struct block)); // if we accommodate for block struct now, it saves us (or creates) a headache later

    // Get best fit within block list:
    //TODO account for no block found.
    struct block* best = find_best_fit(rsize); //find best fit for data + meta.
    // Test if we found a block at all:
    if (best==NULL){
        return(NULL);
    }else{
    //if best block is Goldilocks-block; we are done:
    if(best->size == rsize){
        // mark as not free:
        best->free=false;
        //return ptr to start of data
        return(&(best->data));
    }

    // if best block is bigger than required size; Try splitting:
    while( (best->size > rsize) && (is_splittable(best,rsize)) ){ //While block is bigger than requested size, and is splittable->split: 
            block_split(best);  //split block (see function block_split below).
            // best should be changed by block_split, hopefully the 'best' in the while condition reflects this.
    }
    // Now that best block has gone through all possible splits, we can return the address to start of data:
    if (!is_splittable(best,rsize)){
        best->free=false;
        return(&(best->data));
    }
}
    //return NULL; // If no block could be allocated return NULL . TODO: Double check.
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
    printf("--END--\n"); //TODO:why isn't this within the function? -> good spot, was missing } to end fn
}









