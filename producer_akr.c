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

// //init function for the semaphores
// int sem_init(sem_t *sem, int pshared, unsigned int value);
// // sem_t empty; //number of empty buffers, defined in .h
// // sem_t full; //number of messages in buffer, defined in .h
// // bool lock; //mutex, prevents threads unauthorized from using buffer, defined in .h
// // int BUFFER_SIZE = 10; //defined in .h
// // int msgRemain = 0; //number remaining messages, defined in .h
// // typedef struct {
// //   int itm;
// //   char msg[25];
// // } items;
// // items* AKR; //defined in .h
// int nextOpen = 0; //next open spot in buffer
// // int firstFull = 0; //first full spot in buffer, defined in .h
// int TEN = 10;
// int FALSEBOOL = 0;
// int TRUEBOOL = 1;
// int item = 1;
// // const int SIZE = 4096;
// // int shm_fd;
// // void *ptr;
// int bufferNum = 0;

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
// void *produce(void *ptr){
//   char *in = (char*)ptr;
void *produce(){
  // printf("IN => %p, ptr => %p\n", &in, &ptr);
  pid_t tid = syscall(SYS_gettid);
  int msgProd = 0;
  while (msgRemain > 0) {
    char message[16]; //message to be added to the buffer
    snprintf(message, msgLen, "%d_msg_%d%s", tid, ++msgProd, "\0\0\0\0");
    snprintf(in, msgLen, "%s", message);
    printf("in: %s\n", (char*)in);
    //entry section
    sem_wait(empty);
    sem_wait(lock);
    //critical section
    --msgRemain;
    printf("Producer %d produced item %s (%d/%d)\n", tid, (char*)in, (NUM_MESSAGES - msgRemain), NUM_MESSAGES);
    in += (msgLen % SIZE);
    // printf("IN => %p, ptr => %p\n", &in, &ptr);
    //exit section
    sem_post(lock);
    sem_post(full);
    sleep(1);
  }
  pthread_exit(0);
//  char message[] = ""; //message to be added to the buffer
//  int PID = getpid(); //producer id
//  int proNum = *((int*)arg); //producer number
// //while there are still messages to add, keep running
//  while(msgRemain != 0) {
//    char stringNum[20]; //string to hold the PID and the item number
//    sprintf(stringNum, "%d", PID); //adds PID to the string holder
//    strcpy(message, stringNum); //adds PID to the message
//    strcat(message, "_msg_\0"); //adds test to message
//    sprintf(stringNum, "%d", item); //adds item to the string holder
//    strcat(message, stringNum); //adds the item to the message
//    //entry section
//   //  wait();
//    //critical section
//    items next_produced; //creates item to add to buffer
//    next_produced.itm = item; //sets item number in item
//    strcpy(next_produced.msg, message); //adds string message into item
//    AKR[nextOpen] = next_produced; //adds item into buffer
//    int buffer = nextOpen;
//   //  sprintf(ptr, "%s", message);
//   //  ptr += strlen(message);
//   //  int buffer = bufferNum;
//    //exit section
//   //  signal();
//    //remainder section
//   //  printf("buffer: %i\n", bufferNum);
//    printf("Producer %i produced item %i on buffer %i\n", proNum, item, buffer);
//    item++; //increment item number
//  }
//  pthread_exit(0); //exit thread
}

// /*
// * waits until the buffer is no longer locked and buffer is not full
// *    @return void : nothing gets returned
// */
// void wait(){
//   int fValue; //full semaphore value
//   sem_getvalue(full, &fValue); //gets the value of the semaphore
//   // if (&lock){
//   //   printf("lock: true || ");
//   // } else {
//   //   printf("lock: false || ");
//   // }
//   // printf("fValue == BUFFER_SIZE: %i == %i\n", fValue, (int)*BUFFER_SIZE);
//   // while(&lock || fValue == (int)*BUFFER_SIZE){
//   //   //busy wait
//   //   sleep(1);
//   //   printf("WHILE\n");
//   //   if (&lock){
//   //     printf("lock: true || ");
//   //   } else {
//   //     printf("lock: false || ");
//   //   }
//   //   printf("fValue == BUFFER_SIZE: %i == %i\n", fValue, (int)*BUFFER_SIZE);
//   // }
//   // lock = &TRUEBOOL;
//   // printf("WAIT\n");
//   // if (&lock){
//   //   printf("lock: true\n");
//   // } else {
//   //   printf("lock: false\n");
//   // }
//   //using mutex
//   while(pthread_mutex_trylock(lock) != 0 || fValue == (int)*BUFFER_SIZE){
//     //busy wait
//     sleep(1);
//     printf("WHILE\n");
//     printf("fValue == BUFFER_SIZE: %i == %i\n", fValue, (int)*BUFFER_SIZE);
//   }
//   pthread_mutex_lock(lock);
//   printf("WAIT\n");
// }

// /*
// * increments values and changes where the nextOpen pointer goes, also unlocks
// * the lock on the buffer
// *    @return void : nothing gets returned
// */
// void signal(){
//   nextOpen++; //pointer to the next open spot in the buffer
//   if (nextOpen == (int)*BUFFER_SIZE){
//     nextOpen = 0;
//   }
//   // bufferNum++; //pointer to the next open spot in the buffer
//   // // printf("bufferNum, bufferSize: %i, %i\n", bufferNum, BUFFER_SIZE);
//   // if (bufferNum == (int)*BUFFER_SIZE){
//   //   bufferNum = 0;
//   // }
//   msgRemain--;
//   // int fValue; //full semaphore value
//   // sem_getvalue(full, &fValue); //gets the value of the semaphore
//   // printf("full before inc: %i\n", fValue);
//   sem_post(full); //increment
//   // fValue; //full semaphore value
//   // sem_getvalue(full, &fValue); //gets the value of the semaphore
//   // printf("full after inc: %i\n", fValue);
//   sem_trywait(empty); //decrement
//   // lock = &FALSEBOOL; //release lock
//   pthread_mutex_unlock(lock);
//   printf("SIGNAL\n");
//   // if (lock){
//   //   printf("lock: true\n");
//   // } else {
//   //   printf("lock: false\n");
//   // }
//   sleep(1); //sleep for one second
// }

/*
* sends data from producer to comsumer through shared memory
*/
void sendVars() {
  printf("sendVars\n");
  printf("BUFFER_SIZE: %d\n", BUFFER_SIZE);
  char * fifo = "/tmp/FIFO";
  // mkfifo(fifo, 0666);
  int status = mkfifo(fifo, 0666);
  if (status < 0){
    printf("ERROR mkfifo(): %s\n", strerror(errno));
  } else {
    printf("File Created Successfully\n");
  }
  // int fd = open(fifo, O_WRONLY);
  int fd = open(fifo, O_WRONLY | O_CREAT);
  if (fd < 0){
    printf("ERROR open(): %s\n", strerror(errno));
  } else {
    printf("File Opened Successfully\n");
  }
  char BUFstr[3];
  // sprintf(BUFstr, "%d", BUFFER_SIZE);
  snprintf(BUFstr, 3, "%d", BUFFER_SIZE);
  printf("BUFstr: %s\n", BUFstr);
  int num = write(fd, BUFstr, sizeof(BUFstr));
  if (num < 0){
    printf("ERROR write(): %s\n", strerror(errno));
  } else {
    printf("File Wrote To Successfully\n");
  }
  // write(fd, BUFstr, sizeof(BUFstr));
  // write(fd, BUFstr, strlen(BUFstr)+1);
  printf("wrote to file\n");
  close(fd);
  printf("closed file\n");
  unlink(fifo);
  printf("unlinked file\n");
  printf("\n");
}

/*
* sets up the shared memory object and producer threads
*    @param int : the number of arguments being brought into main
*    @param const char*[] : array of arguments being brought into main
*    @return int : 0 = successful completion
*/
int main(int argc, const char* argv[]){
  printf("main\n");
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
  printf("semaphores open\n");
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
    printf("variables set\n");
    // printf("BUFFER_SIZE: %d\n", BUFFER_SIZE);
    // printf("NUM_PRODUCERS: %d\n", NUM_PRODUCERS);
    // printf("NUM_MESSAGES: %d\n", NUM_MESSAGES);
    sendVars();
    printf("vars sent\n");
    //initialize shared memory
    int shm_fd = shm_open("AKR", O_CREAT | O_RDWR, 0666);
    SIZE = BUFFER_SIZE * sizeof(char)*msgLen;
    ftruncate(shm_fd, SIZE);
    in = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (in == MAP_FAILED){
      printf("mmap failed");
      exit(-1);
    }
    printf("shared memory created\n");
    //initialize and join producer threads
    pthread_t tid[NUM_PRODUCERS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i = 0; i < NUM_PRODUCERS; i++){
      // pthread_create(&tid[i], &attr, produce, in);
      // if (!pthread_create(&tid[i], &attr, produce, in)) {
      //   printf("Thread created successfully\n");
      // } else {
      //   printf("Thread Creation Failed\n");
      // }
      if (!pthread_create(&tid[i], &attr, produce, NULL)) {
        printf("Thread created successfully\n");
      } else {
        printf("Thread Creation Failed\n");
      }
    }
    for (int i = 0; i < NUM_PRODUCERS; i++){
      pthread_join(tid[i], NULL);
    }
    // sem_unlink("empty");
    // sem_unlink("full");
    // sem_unlink("lock");
    // shm_unlink("AKR");
    // unlink("/tmp/FIFO");
  } else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
    helpMessage();
  } else if (argc > 1) {
    printf("WARNING: number of parameters is not valid\n");
    helpMessage();
  }
  // BUFFER_SIZE = &TEN;
  // int NUM_PRODUCERS = 5;
  // int NUM_MESSAGES = 100;
  // //always one argc
  // if (argc == 4){
  //   //verify these are all numbers!
  //   char number[100] = ""; //when left without the 100 it threw a stack smashing error
  //   for(int i = 1; i <= 3; i++){
  //     strcpy(number, argv[i]);
  //     bool isNum = false;
  //     isNum = isNumber(number); //check if it is a number
  //     if(!isNum){
  //       printf("WARNING: value of parameters are not valid\n");
  //       helpMessage();
  //     }
  //   }
  //   int BUFSIZE = atoi(argv[1]);
  //   BUFFER_SIZE = &BUFSIZE;
  //   NUM_PRODUCERS = atoi(argv[2]);
  //   NUM_MESSAGES = atoi(argv[3]);
  // } else if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
  //   helpMessage();
  // } else if (argc > 1){
  //   printf("WARNING: number of parameters is not valid\n");
  //   helpMessage();
  // }
  // msgRemain = &NUM_MESSAGES;
  // // const char *name = "AKR";
  // // shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  // // ftruncate(shm_fd,SIZE);
  // // ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  // // if (ptr == MAP_FAILED){
  // //   printf("Map Failed\n");
  // // }
  // AKR = malloc((int)*BUFFER_SIZE); //set shared memory object size to buffer size
  // int retE;
  // sem_t emptySem;
  // empty = &emptySem;
  // int retF;
  // sem_t fullSem;
  // full = &fullSem;
  // int MAXSIZE = (int)*BUFFER_SIZE;
  // retE = sem_init(empty, 1, MAXSIZE);
  // retF = sem_init(full, 1, 0);
  // int retL;
  // pthread_mutex_t lockMut;
  // lock = &lockMut;
  // retL = pthread_mutex_init(lock, NULL);
  // pthread_t tid;
  // //create pthreads
  // for (int i = 1; i <= NUM_PRODUCERS; i++){
  //   int *arg = malloc(sizeof(*arg));
  //   if(arg == NULL){
  //     printf("Couldnt allocate memory");
  //   }
  //   *arg = i;
  //   // pthread_create(&tid, &attr, produce, &i);
  //   if (!pthread_create(&tid, NULL, produce, arg)) {
  //     // printf("Thread created successfully\n");
  //   }
  // }
  // int val;
  // sem_getvalue(full, &val);
  // //if there are still messages do not exit
  // while(msgRemain != 0){
  //   sleep(3);
  //   sem_getvalue(full, &val);
  // }
  return(0);
}
