/*
* Ashley Roselius
* CEG 4350 Fall 2017
* Project 1: Producer / Consumer Problem
* This program contains function, variable, and structure definitions to be
* used in the producer_akr.c and consumer_akr.c files.
*/

// #ifndef PRODUCER_AKR_H
// #define PRODUCER_AKR_H

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
unsigned int NUM_PRODUCERS = 5;
unsigned int NUM_MESSAGES = 100;
unsigned int msgRemain;
unsigned int msgLen = 16;
unsigned int SIZE = 4069;
void *in;
sem_t * empty;
sem_t * full;
sem_t * lock;

// typedef struct {
//   int itm;
//   char msg[25];
// } items;
//
// sem_t *empty; //number of empty buffers
// extern sem_t *empty;
// sem_t *full; //number of messages in buffer
// extern sem_t *full;
// // int *lock; //mutex, prevents threads unauthorized from using buffer
// // extern int *lock;
// pthread_mutex_t *lock;
// extern pthread_mutex_t *lock;
// int *BUFFER_SIZE = 0;
// extern int *BUFFER_SIZE;
// int *msgRemain = 0; //number remaining messages
// extern int *msgRemain;
// items* AKR;
// extern items* AKR;
// int firstFull = 0;
// extern int firstFull; //first full spot in buffer

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
* The function run by each thread to produce the item and adds it to the
* shared memory object.
*    @param void* : the producer number that is using the function
*    @return void* : nothing gets returned
*/
// void *produce(void *ptr);
void *produce();

// /*
// * waits until the buffer is no longer locked and buffer is not full
// *    @return void : nothing gets returned
// */
// void wait();
//
// /*
// * increments values and changes where the nextOpen pointer goes, also unlocks
// * the lock on the buffer
// *    @return void : nothing gets returned
// */
// void signal();

// #endif
