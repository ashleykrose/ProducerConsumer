/*
* Ashley Roselius
* CEG 4350 Fall 2017
* Project 1: Producer / Consumer Problem
* This program creates threads called consumers to consume items and messages
* from a shared memory object. While one producer or consumer is reading or
* writing to the shared memory object it is locked so no others
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

#include "consumer_akr.h"

/*
* determins if the passed in parameter is a number or not
*    @param char[] : the char array of an input value
*    @return bool : true = number, false = not number
*/
bool isNumber(char num[]){
  int length = strlen(num); //length of the array
  bool isNum = true; //holds if it is a number or not
  for(int i = 0; i < length; i++){
    if (!isdigit(num[i])) {
      isNum = false;
    }
  }
  //check if it is a number
  if (isNum) {
    //make sure its between 1 and 100
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
 printf("\nThis program creates consumers to consume items from a buffer.\n");
 printf("Start the program by typing in the following code:\n");
 printf("     ./consumer_akr [Param 1] [Param 2]\n");
 printf("Param 1: Number of consumers (default = 5)\n");
 printf("Param 2: Number of messages (default = 100)\n\n");
 exit(2);
}

/*
* The function run by each thread to consume the item from a shared memory
* object.
*    @param void* : the consumer number that is using the function
*    @return void* : nothing gets returned
*/
// void *consume(void *ptr){
//   char *out = (char*)ptr;
void *consume(){
  // printf("OUT => %p, ptr => %p\n", &out, &ptr);
  pid_t tid = syscall(SYS_gettid);
  do {
    char message[16];
    //entry section
    int errFWait = sem_wait(full);
    if (errFWait < 0){
      printf("ERROR sem_wait(full): %s\n", strerror(errno));
    }
    int errLWait = sem_wait(lock);
    if (errLWait < 0){
      printf("ERROR sem_wait(lock): %s\n", strerror(errno));
    }
    //critical section
    --msgToConsume;
    strncpy(message, out, msgLen);
    // printf("     Out Address Before: %p\n", (void*)out);
    printf("Consumer %d consumed item %s (%d/%d)\n", tid, message, (NUM_CMESSAGES - msgToConsume), NUM_CMESSAGES);
    out += (msgLen % SIZE);
    // printf("OUT => %p, ptr => %p\n", &out, &ptr);
    // printf("     Out Address After: %p\n", (void*)out);
    //exit section
    int errLPost = sem_post(lock);
    if (errLPost < 0){
      printf("ERROR sem_post(lock): %s\n", strerror(errno));
    }
    int errEPost = sem_post(empty);
    if (errEPost < 0){
      printf("ERROR sem_post(empty): %s\n", strerror(errno));
    }
    sleep(1);
    printf("MSGTOCONSUME (%d): %d\n", tid, msgToConsume);
  } while (msgToConsume > 0);
  printf("after wile loop\n");
  pthread_exit(0);
}

/*
* recieves vars from producer in shared memory
*/
void receiveVars(){
  char * fifo = "/tmp/FIFO";
  char buf[3];
  // int fd = open(fifo, O_RDONLY);
  int fd = open(fifo, O_RDONLY);
  if (fd < 0){
    printf("ERROR open(): %s\n", strerror(errno));
  }
  int rSize = read(fd, buf, 3);
  if (rSize < 0){
    printf("ERROR read(): %s\n", strerror(errno));
  }
  int buffersize = atoi(buf);
  BUFFER_SIZE = buffersize;
  close(fd);
}

/*
* sets up the consumer threads
*    @param int : the number of arguments being brought into main
*    @param const char*[] : array of arguments being brought into main
*    @return int : 0 = successful completion
*/
int main(int argc, const char* argv[]){
  //check if producer is running, if it isnt then exit and print error message
  int numProcess;
  FILE *fp = popen("ps -efa | grep producer | wc -l", "r");
  fscanf(fp, "%d", &numProcess);
  pclose(fp);
  if (numProcess == 2) {
    printf("The Producer is not currently running.\n");
    printf("Please start the producer before starting this program.\n");
  } else {
    printf("The Producer is running\n");
    if (argc == 3 || argc == 1) {
      printf("Valid values\n");
      int NUM_CONS = 5;
      int NUM_CMESS = 100;
      msgLen = 16;
      if (argc == 3) {
        printf("Values sent in\n");
        //verify these are all numbers!
        char number[100] = ""; //when left without the 100 it threw a stack smashing error
        printf("check values\n");
        for(int i = 1; i < 3; i++){
          strcpy(number, argv[i]);
          printf("copied string\n");
          bool isNum = false;
          isNum = isNumber(number); //check if it is a number
          if(!isNum){
            printf("WARNING: value of parameters are not valid\n");
            helpMessage();
          }
        }
        printf("numbers checked\n");
        NUM_CONS = atoi(argv[1]);
        NUM_CMESS = atoi(argv[2]);
        printf("Set Values\n");
      }
      NUM_CONSUMERS = NUM_CONS;
      NUM_CMESSAGES = NUM_CMESS;
      msgToConsume = NUM_CMESS;
      printf("Set Global Variables\n");
      receiveVars();
      //open semaphores
      printf("Opening Sems\n");
      empty = sem_open("empty", 0);
      if (empty <= 0){
        printf("ERROR empty sem_open(): %s\n", strerror(errno));
      }
      full = sem_open("full", 0);
      if (full <= 0){
        printf("ERROR full sem_open(): %s\n", strerror(errno));
      }
      lock = sem_open("lock", 1);
      if (lock <= 0){
        printf("ERROR lock sem_open(): %s\n", strerror(errno));
      }
      printf("Opened Semaphores\n");
      //initialize shared memory
      printf("initializing Memory\n");
      int shm_fd = shm_open("AKR", O_CREAT | O_RDWR, 0666);
      SIZE = BUFFER_SIZE * sizeof(char)*msgLen;
      out = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
      if (out == MAP_FAILED){
        printf("out map failed\n");
        printf("Error waiting on empty %s\n", strerror(errno));
        exit(-1);
      }
      printf("initialized Memory\n");
      //initialize and join producer threads
      pthread_t tid[NUM_CONSUMERS];
      for (int i = 0; i < NUM_CONSUMERS; i++){
        printf("tid[%d]: %lu\n", i, tid[i]);
      }
      pthread_attr_t attr;
      pthread_attr_init(&attr);
      for (int i = 0; i < NUM_CONSUMERS; i++){
        // if (!pthread_create(&tid[i], &attr, consume, out)) {
        //   printf("Thread created successfully\n");
        // }
        if (!pthread_create(&tid[i], &attr, consume, NULL)) {
          printf("Thread created successfully\n");
        }
      }
      for (int i = 0; i < NUM_CONSUMERS; i++){
        pthread_join(tid[i], NULL);
      }
      sem_unlink("empty");
      sem_unlink("full");
      sem_unlink("lock");
      shm_unlink("AKR");
    } else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
      helpMessage();
    } else if (argc > 1) {
      printf("WARNING: number of parameters is not valid\n");
      helpMessage();
    }
  }
  return(0);
}
