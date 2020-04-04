/*************************************************
 * Id: oste1799
 * Purpose:
 *  1st step in solving the procuder and comsumer problem
 * Input:
 *  Start time, number of producers, number of consumers,
 *  max sleep time between writes, max sleep time between reads
 * Output:
 *  Is dependent on user input but show producers writing to buffer
 * Compile:  gcc -g -Wall  -o test1 IC19.c
 *************************************************************/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
//protoypes
pid_t forkProducer (int index, int fd_p[], int tMaxSleep, const char write_char);
pid_t forkConsumer(int index, int fd_p[], int tMaxSleep, const char write_char);
void runB(time_t tStart, int iNumProducers, int iNumConsumers, int **fd_p, int **fd_c, int tRunTime, pid_t *children); 
int main(int argc, char* argv[])
{
   int tRunTime;
   int iNumProducers;
   int tMaxProducerSleep;
   int iNumConsumers;
   int tMaxConsumerSleep;
   time_t start = time(NULL);
   tRunTime = atoi(argv[1]);   //The time in seconds the program will run
   printf("Run Time = %d\n", tRunTime);
   fflush(stdout);
   iNumProducers = atoi(argv[2]);   //The number of producers
   printf("Num of producers %d\n", iNumProducers);
   fflush(stdout);
   tMaxProducerSleep = atoi(argv[3]);   //The maximum sleep time between writes for a producer
   printf ("Max sleep time between producers %d\n", tMaxProducerSleep);
   fflush(stdout);
   iNumConsumers = atoi(argv[4]);   //The number of Consumers
   printf ("Number of consumers %d\n", iNumConsumers);
   fflush(stdout);
   tMaxConsumerSleep = atoi(argv[5]);   //The max sleep time between reads for a consumer
   printf ("Max sleep time between consumers %d\n", tMaxConsumerSleep);
   fflush(stdout);
   //ic_19 run_time num_producers max_sleep_producer num_consumers max_sleep_consumer
   //ic_19 10 4 45 4 72
   //Example input to program and the order of inputs
   //for loop for forkProducer
   pid_t *children=(pid_t*)malloc((iNumProducers+ iNumConsumers) * sizeof(pid_t));  //Producers
   int **fd_p=(int**)malloc(iNumProducers * sizeof(int*));  //Producers
   for (int i =0; i < iNumProducers; i++){
      fd_p[i] = (int *) malloc (2*sizeof(int));
      children[i] = forkProducer(i, fd_p[i], tMaxProducerSleep, 'A'+i);
   }
/*   //for loop for forkConsumer
   int **fd_c=(int**)malloc(iNumConsumers * sizeof(int*));
   for (int j=0; j < iNumConsumers; j++){
      fd_c[j] = (int *) malloc (2*sizeof(int));
      children[j] = forkConsumer(j, fd_c[j], tMaxConsunerSleep, 'A' +j);
   }
   runB(start, iNumProducers, iNumConsumers, fd_p, fd_c, tRunTime, children);
*/
   runB(start, iNumProducers, iNumConsumers, fd_p, fd_p, tRunTime, children);
   return(0);
}
// Function to create fork producer
pid_t forkProducer (int index, int fd_p[], int tMaxSleep, const char write_char)
{
   //Step #3
   int pipesize =32;
   char write_str[2];
   write_str[1] = 0;
   write_str[0] = write_char;
   //===============================Do in forkProducer
   pipe(fd_p);
   pid_t childpid = fork();
   if((childpid = fork()) == -1)
   {
      perror("fork");
      exit(1);
   }
   if(childpid == 0)    //child process
   {
      fcntl(fd_p[1], F_SETPIPE_SZ, &pipesize);   //STEP #3**
      /* Child process closes up input side of pipe */
      close(fd_p[0]);
      /* Send "string" through the output side of pipe */
      //Note #4**
      while (1){
         usleep(rand() % tMaxSleep * 1000);
         write(fd_p[1], write_str, (strlen(write_str)+1));
      }
      exit(0);
   }
   else   //parent
   {
      fcntl(fd_p[0], F_SETPIPE_SZ, &pipesize);
      close(fd_p[1]);
      return childpid;
   }
}
//function to create fork consumers
pid_t forkConsumer(int index, int fd_p[], int tMaxSleep, const char write_char){
   return 0;
}
//RunB function to loop through the recieved string
void runB(time_t tStart, int iNumProducers, int iNumConsumers, int **fd_p, int **fd_c, int tRunTime, pid_t *children){
   int nbytes;   //variables
   char readbuffer[2];
   //Note #3** //do same thing for consumers exceot use 1 instead of 0
   for (int i =0; i < iNumProducers; i++){
      fcntl(fd_p[i][0], F_SETFL, O_NONBLOCK);
   }
   while ((time(NULL)- tStart) < tRunTime){
      //Given*****************
      for (int i =0; i < iNumProducers; i++){
         nbytes = read(fd_p[i][0], readbuffer, sizeof(readbuffer));
         if (nbytes > 0 ){
            printf("Received string: %s from P%d\n", readbuffer, i);
            fflush(stdout);
         }
      }
   }
   //Note #2**
   for(int i =0; i < iNumProducers; i++){
      kill(children[i], SIGKILL);
   }
}
