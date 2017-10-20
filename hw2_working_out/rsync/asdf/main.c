// main.c

/*
 *  This is a simple program for your custom malloc. Only the most basic cases are tested,
 *  you are encouraged to add more test cases to ensure your functions work correctly.
 *  Having absolutely everything in this file working does not indicate that you have fully
 *  completed the assignment. You need to implement everything specified in the brief.
 *
 *  A result of 'done' does NOT indicate that the function works entirely correctly, only that
 *  the operation did not create fatal errors.
 */

#include "allocator.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
   /* int *data,i;
    double *ddata;
    bool done;
   */
    printf("\nStarting tests...\n");
	printf("-- NOTE: Once you failed a test the following tests aren't accurate --\n");
	printf("______________________________________________________\n");
    printf("Custom_malloc Tests\n");
    printf("______________________________________________________\n");

    //ddata = (double*) custom_malloc(sizeof(double));
    //*ddata = 12345.6789;
    //printf("%s\n",(*ddata)==12345.6789?"'done'":"failed!");
    //printf("Block size: %i\n", sizeof(struct block));

    int test = 0;
    int* testGroup [512];
    bool flag = true;



    testGroup[0] = (int*)custom_malloc(900);
    *testGroup[0] = 0;

    struct block* tempBlock = (void*)testGroup[0] - sizeof(struct block);

	//Test confirm bad difference between block pointer address and block->data
	printf("Test %i %s: bad difference between block pointer address and block->data \n", test++, tempBlock->data == testGroup[0] ?"passed":"failed");

	//Test confirm input rounded up to the closest power of 2
	printf("Test %i %s: input size rounded up to the closest power of 2\n", test++, tempBlock->size == 1024 || tempBlock->size == 2048 ?"passed":"failed");


    //Test confirm input is inclusive of block size befor being rounded

    printf("Test %i %s: block size is added to size requested before rounding\n", test++, tempBlock->size == 2048 ?"passed":"failed");

    //Test return of Null when a blockof size 0 is requested
    printf("Test %i %s: return of Null when a block of size 0 is requested\n", test++, custom_malloc(0)==NULL ? "passed" : "failed");

    //Test Linked list correctly set up
    flag = true;

    struct block* curr = head;
    int size = 0;
    while(curr != NULL){

		size++;
		curr = curr->next;

	}

	printf("Test %i %s: Linked list correctly set up\n", test++, size == 10 ? "passed" : "failed");


    //Test Blocks correctly split up after one memory request
    flag = true;

    size_t currSize = 2048;
    curr = head;

    if(curr->size != currSize){

    	flag = false;
    }
    curr = curr->next;

    while(flag && (curr != NULL)){


    	if(curr->size != currSize){
    		flag = false;
    		break;
    	}

    	currSize *= 2;
    	curr = curr->next;

    }

    printf("Test %i %s: Blocks correctly split up after one memory request\n", test++, flag ? "passed" : "failed");


    //Test assignments of 512 x 2048 blocks filling memmory\n

    for (int i = 1; i < 512; i++){
    	testGroup[i] = (int*)custom_malloc(1800);
    	*testGroup[i] = i;
    }
    //print_list();

    flag = true;
    //print_list();
    for (int i = 0; i < 512; i++){
	//printf("Index %d has value %d \n", i, *testGroup[i]);
    	if((*testGroup[i]) != i){
    		printf("Test %i failed: assignments of 512 x 2048 blocks filling memmory\n", test++);
    		flag = false;
		break;
    	}

    }

    if(flag){

    	printf("Test %i passed: assignments of 512 x 2048 blocks filling memmory\n", test++);
    }
    	

    //Test return of null when there is not enough space
    printf("Test %i %s: return of Null when there is no space in memory\n", test++, custom_malloc(100) == NULL ? "passed" : "failed");


	//Test free
    printf("\nCustom_Free Tests\n");
    printf("______________________________________________________\n");

	//check if free frees tail block
	flag = true;
	tempBlock = (void*)testGroup[511] - sizeof(struct block);
	custom_free(testGroup[511]);
	if (!tempBlock->free){
		flag = false;
	} else {
		flag = (custom_malloc(1800) != NULL);
	}
	printf("Test %i %s: check if free frees tail block\n", test++, flag ? "passed" : "failed");

	//check if free frees head block
	flag = true;
	tempBlock = (void*)testGroup[0] - sizeof(struct block);
	custom_free(testGroup[0]);
	if (!tempBlock->free){
		flag = false;
	} else {
		flag = (custom_malloc(1800) != NULL);
	}
	printf("Test %i %s: check if free frees head block\n", test++, flag ? "passed" : "failed");

	//check if free frees middle block
	flag = true;
	tempBlock = (void*)testGroup[42] - sizeof(struct block);
	custom_free(testGroup[42]);
	if (!tempBlock->free){
		flag = false;
	} else {
		flag = (custom_malloc(1800) != NULL);
	}
	printf("Test %i %s: check if free frees middle block\n", test++, flag ? "passed" : "failed");

	//check if free frees neighbor blocks - left block first
	flag = true;
	struct block* tempBlockLeft = (void*)testGroup[42] - sizeof(struct block);
	struct block* tempBlockRight = (void*)testGroup[43] - sizeof(struct block);
	custom_free(testGroup[42]);
	custom_free(testGroup[43]);
	if (!tempBlockLeft->free){
		flag = false;
	} else if(tempBlockLeft->next != tempBlockRight->next) {
		flag = false;
	} else {
		flag = (custom_malloc(3800) != NULL);
	}
	printf("Test %i %s: check if free merges neighbor blocks - left block freed first\n", test++, flag ? "passed" : "failed");

	//check if free frees neighbor blocks - right block first
	flag = true;
	tempBlockLeft = (void*)testGroup[16] - sizeof(struct block);
	tempBlockRight = (void*)testGroup[17] - sizeof(struct block);
	custom_free(testGroup[17]);
	custom_free(testGroup[16]);
	if (!tempBlockLeft->free){
		flag = false;
	} else if(tempBlockLeft->next != tempBlockRight->next) {
		flag = false;
	} else {
		flag = (custom_malloc(3800) != NULL);
	}
	printf("Test %i %s: check if free merges neighbor blocks - right block freed first\n", test++, flag ? "passed" : "failed");

	//check that blocks are allocated using best fit and not first fit
	struct block* tempBlockLarge = (void*)testGroup[42] - sizeof(struct block);
	struct block* tempBlockSmall = (void*)testGroup[44] - sizeof(struct block);
	custom_free(testGroup[42]); //testGroup[42] points to block of size 4096
	custom_free(testGroup[44]);
	custom_malloc(1800);
	printf("Test %i %s: check that blocks are allocated using best fit and not first fit\n", test++, (tempBlockLarge->free && !tempBlockSmall->free) ? "passed" : "failed");

	//test if freeing pointer no longer in list changes list
	flag = true;
	struct block* currBlock = head;
	int val = 0;
	while (currBlock){
		*((int*)currBlock->data) = val++;
		currBlock = currBlock->next;
	}
	custom_free(testGroup[43]);
	currBlock = head;
	val = 0;
	while (currBlock){
		if (*((int*)currBlock->data) != val++){
			flag = false;
			break;
		}
		currBlock = currBlock->next;
	}
	printf("Test %i %s: test if freeing pointer no longer in list changes list\n", test++, flag ? "passed" : "failed");

	//test if freeing NULL changes list
	flag = true;
	currBlock = head;
	val = 0;
	while (currBlock){
		*((int*)currBlock->data) = val++;
		currBlock = currBlock->next;
	}
	custom_free(NULL);
	currBlock = head;
	val = 0;
	while (currBlock){
		if (*((int*)currBlock->data) != val++){
			flag = false;
			break;
		}
		currBlock = currBlock->next;
	}
	printf("Test %i %s: test if freeing NULL changes list\n", test++, flag ? "passed" : "failed");


    printf("\nCustom_Realloc Tests\n");
    printf("______________________________________________________\n");
    
    //check if correctly reallocs memory to a larger size

    tempBlock = (void*)testGroup[44] - sizeof(struct block);
    struct block* newBlock = (void *)testGroup[42] - sizeof(struct block); 	// it should be moved here as this 
    																		//is the only available free space of large enough sized
    custom_realloc(testGroup[44], 3800);
    
    flag = true;

    if(*((int*)newBlock->data) != *((int*)tempBlock->data)){
    
    	flag = false;
    }else if(newBlock->size != 4096){
    
    	flag = false;
    }
    
    
    printf("Test %i %s: correctly reallocs to a larger size\n", test++, flag ? "passed" : "failed");
    
    //check that it frees a memory location after realloc called to a larger size
    
    printf("Test %i %s: frees a memory location after realloc called to a larger size\n", test++, tempBlock->free && !newBlock->free ? "passed" : "failed");
    
    //check that it correctly reallocs to a smaller size
    int* tempInt = custom_realloc(testGroup[42], 1800);
   
    printf("Test %i %s: correctly reallocs to a smaller size\n", test++, tempInt == testGroup[42] && *tempInt == *testGroup[42]  ? "passed" : "failed");
    
    //check that it correctly splits the memory after realloc to a smaller size
    
    newBlock = (void *)testGroup[42] - sizeof(struct block);
    tempBlock = (void *)testGroup[43] - sizeof(struct block); 	//this points to where the second block of the split should be
    
    flag = newBlock->size == 2048 && newBlock->next->data == tempBlock->data && tempBlock->free; 
    
    printf("Test %i %s: correctly reallocs to a smaller size\n", test++,  flag ? "passed" : "failed");
       
    //checks if realloc returns NULL if passed a NULL pointer
    printf("Test %i %s: return of Null when function is passed a NULL pointer\n", test++, custom_realloc(NULL, 42) == NULL ? "passed" : "failed");
    
    //free all the memory
    
    for(int i = 0; i < 512; i++){
    
    	custom_free(testGroup[i]);
    }
    
    printf("Test %i %s: free up all the memory\n", test++, head->next == NULL && head->size == MAX_SIZE && head->free ? "passed" : "failed");
    
    printf("______________________________________________________\n");
    printf("\n\nWell done for not crashing, Enjoy Craigs tests....\n");
	printf("______________________________________________________\n");
    
    //craigs tests


   	int *data,i;
    double *ddata;
    bool done;
   
    printf("\n\nStarting Craigs tests...\n");
	printf("______________________________________________________\n");
    printf("Custom_malloc...");

    ddata = (double*) custom_malloc(sizeof(double));
    *ddata = 12345.6789;
    printf("%s\n",(*ddata)==12345.6789?"'done'":"failed!");


    printf("Array malloc....");
    data = (int*) custom_malloc(1028*sizeof(int));
    for(i = 0; i < 1028; i++) data[i] = i;
    done = true;
    for(i = 0; i < 1028; i++) done &= (data[i]==i);
    printf("%s\n",done?"'done'":"failed!");

    printf("Basic free......");
    custom_free(ddata);
    printf("'done'\n");

    printf("Array free......");
    custom_free(data);
    printf("'done'\n");

    printf("Basic realloc...");
    ddata = (double*) custom_malloc(sizeof(double));
    *ddata = 12345.6789;
    double* old = ddata;
    ddata = (double*) custom_realloc(ddata,1000*sizeof(double));
    done = ((old<=ddata)&&((*ddata)==12345.6789));
    custom_free(ddata);
    printf("%s\n",done?"'done'":"failed!");

    return 0;
}
