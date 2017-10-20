// allocator.c
#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

bool hasMallocedBefore = false;

int getNextPowerOfTwo(int number){
	number--;
	number |= number >> 1;
	number |= number >> 2;
	number |= number >> 4;
	number |= number >> 8;
	number |= number >> 16;
	number++;

	return(number);
}

struct block* createBlock(int sizer, struct block* startOfBlock){
	struct block* newBlock = startOfBlock;
	newBlock->size = sizer;
	newBlock->next = NULL;  //Fix here or set afterwards
	newBlock->free = true;
	newBlock->next = NULL;
	newBlock->buddy = NULL;
	newBlock->data = NULL;
	for(int i = 0; i < MAX_EXP; i++){
		newBlock->merge_buddy[i] = NULL;
	}
	return newBlock;
}

/*void splitBlock(struct block* blockToSplit){
	struct block* newBlockOne = createBlock((blockToSplit->size)/2, blockToSplit);
	struct block* newBlockTwo = createBlock((blockToSplit->size)/2, (char*) blockToSplit + (blockToSplit->size/2));
	newBlockOne->data = newBlockOne + sizeof(struct block);		//Pointer to where the blocks metadata ends and where the actual data starts
	newBlockTwo->data = newBlockTwo + sizeof(struct block);
	newBlockOne->buddy = newBlockTwo;
	newBlockTwo->buddy = newBlockOne;
	newBlockTwo->next = blockToSplit->next;
	blockToSplit->next = newBlockOne;
	newBlockOne->next = newBlockTwo;
	newBlockOne->merge_buddy[0] = blockToSplit->buddy;
	newBlockTwo->merge_buddy[0] = blockToSplit->buddy;
}*/

splitBlock(struct block* blockToSplit){
	struct block* newBlock = createBlock((blockToSplit->size)/2, (char*) blockToSplit + (blockToSplit->size/2));
	newBlock->data = (char*) newBlock + sizeof(struct block);
	newBlock->buddy = blockToSplit;
	newBlock->next = blockToSplit->next;
	newBlock->merge_buddy[0] = blockToSplit->buddy;
	int i = 1;
	while((blockToSplit->merge_buddy[i-1] != NULL) && (i < MAX_EXP)){
		newBlock->merge_buddy[i] = blockToSplit->merge_buddy[i-1];
		i++;
	}				    
	blockToSplit->size = blockToSplit->size/2;
	blockToSplit->buddy = newBlock;
	//blockToSplit->merge_buddy[0] = blockToSplit->next;
	blockToSplit->next = newBlock;
	int j = 0;
	while((newBlock->merge_buddy[j] != NULL) && (j < MAX_EXP)){
		blockToSplit->merge_buddy[j] = newBlock->merge_buddy[j];
		j++;
	}
	//printf("Just pointed to: %p\n", blockToSplit->next);
	//printf("This pointer shouldn't change: %p\n", blockToSplit->next->next);
	
}

void* findAndAllocateMemoryOnHeap(int size){
	//printf("Searching for size %d \n", size);
	struct block* correctBlockPointer = NULL;
	int returnSuccessful = 0;
	bool foundCorrectSizeBlock = false;
	struct block* minBlockPointer = NULL;
	while(foundCorrectSizeBlock == false){
		//struct block* iterator = head;
		minBlockPointer = head;
		while(minBlockPointer){
			if((minBlockPointer->size >= size) && (minBlockPointer->free == true)){
				break;
			}
			minBlockPointer = minBlockPointer->next;
		}
		if(((minBlockPointer) && (minBlockPointer->size < size)) || (!minBlockPointer)){
			return(NULL);
		}
		struct block* iterator = minBlockPointer;

		while(iterator){
			if((iterator->size == size) && (iterator->free == true)){
				//printf("Entered first if\n");
				minBlockPointer = iterator;
				foundCorrectSizeBlock = true;
				//correctBlockPointer = iterator;
				break;
			}
			else if((iterator->size < minBlockPointer->size) && (iterator->size > size) && (iterator->free == true)){
				//printf("Entered else if\n");
				minBlockPointer = iterator;
			}
			//printf("%p\n", iterator->next);
			iterator = iterator->next;

			if(foundCorrectSizeBlock == true){
				break;
			}
		}
		if(foundCorrectSizeBlock == false){
			//printf("About to split block of size %d, at address %p\n", minBlockPointer->size, minBlockPointer);				    
			splitBlock(minBlockPointer);
			//printf("After split");
			//print_list();
		}
		else{
			returnSuccessful = 1;
			break;
			/*printf("Have found correct block size\n");
			printf("Block found size is: %d\n", minBlockPointer->size);
			printf("Block at memory: %p\n", minBlockPointer);*/
		}
	}
	//printf("Memory at %p freed\n", minBlockPointer);
	minBlockPointer->free = false;
	return(minBlockPointer->data);
}

/** Allocate a block of at least the requested size **/
void* custom_malloc(size_t size) {
    // TODO
	struct block* findBlock = head;
	void* correctBlockPointer = NULL;
	if((size == 0) || (size > MAX_SIZE)){
		return(NULL);
	}
	else{
		//struct block* wholeWindow = NULL;
		int sizePlusMeta = size + sizeof(struct block);
		int sizePlusMetaNextPowerOfTwo = getNextPowerOfTwo(sizePlusMeta);
		//printf("\nWe want to find a block of size %d \n", sizePlusMetaNextPowerOfTwo);
		if(!hasMallocedBefore){
			head = createBlock(MAX_SIZE, sbrk(MAX_SIZE));	//wholeWindow block must be free as it is not alocated, it is essentially a frame of reference
			//head = wholeWindow;  //sbrk returns the address of the previous break in the heap, before it increased it
			head->data = (char*) head + sizeof(struct block);
			//printf("Head address is: %p\n", head);
			hasMallocedBefore = true;
		}
		correctBlockPointer = findAndAllocateMemoryOnHeap(sizePlusMetaNextPowerOfTwo);
		
		/*if(returnFoundSuccessful == 0){
			return(NULL);
		}*/
		//printf("Head size is: %d\n", head->size);
		findBlock = head;
		struct block* tempBlock = head;
		int counter = 1;
		while(tempBlock->next != NULL){
			counter++;
			tempBlock = tempBlock->next;
		}
		//printf("Size of memory tree is: %d\n", counter);
		/*while(findBlock->size != sizePlusMetaNextPowerOfTwo){
			if(findBlock->next != NULL){
				//printf("Looking at size %d\n",findBlock->size);
				findBlock = findBlock->next;
			}
			else{
				//printf("Couldn't find the block\n");
				break;
			}
		}*/
		//printf("Finished custom mallocing\n");
		//print_list();
	}
    //printf("Returning block at: %p\n", findBlock->data);
    //print_list();	
    return(correctBlockPointer);
}

void* mergeWithBuddy(struct block* block){
	if(block < block->buddy){  //If we are looking at the left block in the buddy pair
		if((block->buddy->free == true) && (block->size == block->buddy->size)){
			//printf("##################################################################################################\n\n");
			block->size = block->size * 2;
			block->free = true;
			block->next = block->buddy->next;
			block->buddy = block->merge_buddy[0];
			int i = 1;
			//block->merge_buddy[i-1] = block->merge_buddy[i+1];
			while((block->merge_buddy[i] != NULL) && (i < MAX_EXP)){
				block->merge_buddy[i-1] = block->merge_buddy[i];
				i++;
			}
			block->merge_buddy[i-1] = NULL;
			return(block);
		}
	}
	else{
		//printf("Inside else \n");
		struct block* holder = block->buddy;
		if((block->buddy->free == true) && (block->size == block->buddy->size)){
			//printf("Entered here \n");
			holder->size = block->size * 2;
                        holder->free = true;
                        holder->next = block->next;
                        holder->buddy = block->merge_buddy[0];
			int i = 1;
                        while((holder->merge_buddy[i] != NULL) && (i < MAX_EXP)){
                                holder->merge_buddy[i-1] = holder->merge_buddy[i];
				i++;
                        }
			holder->merge_buddy[i-1] = NULL;
			return(holder);
		}
	}
	//printf("End of joining blocks");
	return(NULL);
}

/** Mark a data block as free and merge free buddy blocks **/
void custom_free(void* ptr) {
    // TODO
	if(ptr != NULL){
		struct block* holderBlock = (void*) ptr - sizeof(struct block);
		holderBlock->free = true;
		//printf("HolderBlock size is: %d\n", holderBlock->size);
		while((holderBlock != NULL) && (holderBlock->size != MAX_SIZE)){
			holderBlock = mergeWithBuddy(holderBlock);
			//print_list();
		}
		//printf("Finished freeing \n");
	}
	//print_list();
}

/** Change the memory allocation of the data to have at least the requested size **/
void* custom_realloc(void* ptr, size_t size) {
    // TODO
	size_t sizeToUse = getNextPowerOfTwo(size);
	ptr = (double*) ptr;
	if(ptr == NULL){
		struct block* newBlock = custom_malloc(size);
		return(newBlock->data);
	}
	struct block* holderBlock = (void* ) ptr - sizeof(struct block);
	//int sizeToCopy = holderBlock->size - sizeof(struct block) -10;
	int sizeToCopy = sizeof(double);
	if(holderBlock->size == sizeToUse){
		return(holderBlock->data);
	}
	if(sizeToUse < holderBlock->size){
		while(sizeToUse < holderBlock->size){
			splitBlock(holderBlock);
		}
		return(holderBlock->data);
	}
	//print_list();
	custom_free(ptr);
	//print_list();
	void* new = custom_malloc(size);
	memcpy(new, ptr, sizeToCopy);
	return(new);
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

/*---------------------------------------------------------------------Notes---------------------------------------------------------------------
https://stackoverflow.com/questions/14588767/where-in-memory-are-my-variables-stored-in-c
*/
