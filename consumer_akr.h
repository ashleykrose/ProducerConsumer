/*
* Ashley Roselius
* CEG 4350 Fall 2017
* Project 1: Producer / Consumer Problem
* This program contains function and variable definitions to be used in the
* consumer_akr.c file.
*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

unsigned int BUFFER_SIZE = 10;
unsigned int NUM_CONSUMERS;
unsigned int NUM_CMESSAGES;
unsigned int msgToConsume;
unsigned int msgLen = 16;
unsigned int SIZE = 4069;
void *out;
sem_t * empty;
sem_t * full;
sem_t * lock;

/*
* determins if the passed in parameter is a number or not
*    @param char[] : the char array of an input value
*    @return bool : true = number, false = not number
*/
bool isNumber(char num[]);

/*
* prints a help messages
*    @return void : no return value
*/
void helpMessage();

/*
* The function run by each thread to consume the item from a shared memory
* object.
*    @param void* : the consumer number that is using the function
*    @return void* : nothing gets returned
*/
void *consume();
