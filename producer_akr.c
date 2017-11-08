/*
* Ashley Roselius
* CEG 4350 Fall 2017
* Project 1: Producer / Consumer Problem
* This program creates threads called producers to produce items and messages
* and store them in a shared memory object. While one producer or consumer is
* reading or writing to the shared memory object it is locked so no others
* can access it. When it leaves it unlocks it for the next available to access.
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

#include "producer_akr.h"

/*
* determins if the passed in parameter is a number or not
*    @param char[] : the char array of an input value
*    @return bool : true = number, false = not number
*/
bool isNumber(char num[]){
  int length = strlen(num); //length of array
  bool isNum = true; //holds if it is number or not
  for(int i = 0; i < length; i++){
    if (!isdigit(num[i])) {
      isNum = false;
    }
  }
  //check if it is a number
  if (isNum) {
    //check if its between 1 and 100
    if (atoi(num) < 1) {
      isNum = false;
    } else if (atoi(num) > 100) {
      isNum = false;
    }
  }
  return isNum;
}

/*
* prints a help messages
*    @return void : no return value
*/
void helpMessage(){
  printf("\nThis program creates producers to produce items\n");
  printf("   and messages and stores in a buffer.\n");
  printf("Start the program by typing in the following code:\n");
  printf("     ./producer_akr [Param 1] [Param 2] [Param 3]\n");
  printf("Param 1: Size of the buffer (default = 10)\n");
  printf("Param 2: Number of producers (default = 5)\n");
  printf("Param 3: Number of messages (default = 100)\n\n");
  exit(2);
}

/*
* The function run by each thread to produce the item and adds it to the
* shared memory object.
*    @param void* : the producer number that is using the function
*    @return void* : nothing gets returned
*/
void *produce(){
  pid_t tid = syscall(SYS_gettid);
  int msgProd = 0;
  while (msgRemain > 0) {
    char message[16]; //message to be added to the buffer
    snprintf(message, msgLen, "%d_msg_%d%s", tid, ++msgProd, "\0\0\0\0");
    snprintf(in, msgLen, "%s", message);
    //entry section
    sem_wait(empty);
    sem_wait(lock);
    //critical section
    --msgRemain;
    printf("Producer %d produced item %s (%d/%d)\n", tid, (char*)in, (NUM_MESSAGES - msgRemain), NUM_MESSAGES);
    in += (msgLen % SIZE);
    //exit section
    sem_post(lock);
    sem_post(full);
    sleep(1);
  }
  pthread_exit(0);
}

/*
* sends data from producer to comsumer through shared memory
*/
void sendVars() {
  char * fifo = "/tmp/FIFO";
  int status = mkfifo(fifo, 0666);
  if (status < 0){
    printf("ERROR mkfifo(): %s\n", strerror(errno));
  }
  int fd = open(fifo, O_WRONLY | O_CREAT);
  if (fd < 0){
    printf("ERROR open(): %s\n", strerror(errno));
  }
  char BUFstr[3];
  snprintf(BUFstr, 3, "%d", BUFFER_SIZE);
  int num = write(fd, BUFstr, sizeof(BUFstr));
  if (num < 0){
    printf("ERROR write(): %s\n", strerror(errno));
  }
  close(fd);
  unlink(fifo);
}

/*
* sets up the shared memory object and producer threads
*    @param int : the number of arguments being brought into main
*    @param const char*[] : array of arguments being brought into main
*    @return int : 0 = successful completion
*/
int main(int argc, const char* argv[]){
  sem_unlink("empty");
  sem_unlink("full");
  sem_unlink("lock");
  shm_unlink("AKR");
  unlink("/tmp/FIFO");
  //open semaphores
  empty = sem_open("empty", O_CREAT, 0666, BUFFER_SIZE);
  if (empty <= 0){
    printf("ERROR empty sem_open(): %s\n", strerror(errno));
  }
  full = sem_open("full", O_CREAT, 0666, 0);
  if (full <= 0){
    printf("ERROR full sem_open(): %s\n", strerror(errno));
  }
  lock = sem_open("lock", O_CREAT, 0666, 1);
  if (lock <= 0){
    printf("ERROR lock sem_open(): %s\n", strerror(errno));
  }
  if (argc == 4 || argc == 1) {
    int BUFFSIZE = 10;
    int NUM_PROD = 5;
    int NUM_MESS = 100;
    if (argc == 4) {
      //verify these are all numbers!
      char number[100] = ""; //when left without the 100 it threw a stack smashing error
      for(int i = 1; i <= 3; i++){
        strcpy(number, argv[i]);
        bool isNum = false;
        isNum = isNumber(number); //check if it is a number
        if(!isNum){
          printf("WARNING: value of parameters are not valid\n");
          helpMessage();
        }
      }
      BUFFSIZE = atoi(argv[1]);
      NUM_PROD = atoi(argv[2]);
      NUM_MESS = atoi(argv[3]);
    }
    BUFFER_SIZE = BUFFSIZE;
    NUM_PRODUCERS = NUM_PROD;
    NUM_MESSAGES = NUM_MESS;
    msgRemain = NUM_MESS;
    sendVars();
    //initialize shared memory
    int shm_fd = shm_open("AKR", O_CREAT | O_RDWR, 0666);
    SIZE = BUFFER_SIZE * sizeof(char)*msgLen;
    ftruncate(shm_fd, SIZE);
    in = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (in == MAP_FAILED){
      printf("mmap failed");
      exit(-1);
    }
    //initialize and join producer threads
    pthread_t tid[NUM_PRODUCERS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i = 0; i < NUM_PRODUCERS; i++){
      if (!pthread_create(&tid[i], &attr, produce, NULL)) {
        printf("Thread created successfully\n");
      } else {
        printf("Thread Creation Failed\n");
      }
    }
    for (int i = 0; i < NUM_PRODUCERS; i++){
      pthread_join(tid[i], NULL);
    }
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("lock");
    shm_unlink("AKR");
    unlink("/tmp/FIFO");
  } else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
    helpMessage();
  } else if (argc > 1) {
    printf("WARNING: number of parameters is not valid\n");
    helpMessage();
  }
  return(0);
}
