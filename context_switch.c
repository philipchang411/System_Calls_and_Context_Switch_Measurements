#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

/*** Function Declarations ***/
double contextSwitch();
void setCPUAffinity();

/*** Main ***/
int main() {

  // Set the machine to s single processor
  setCPUAffinity(3);

  int trials = 100;

  // Measure cost of a context switch
  double time = contextSwitch(trials);
  printf("Time taken for %d forced context switches: %f\n", trials, time);

  return 0;
}

/*** Function Definitions ***/
/**********************************************/
double contextSwitch(int NUMTRIALS) {
  int p[2];
  int one = 1;
  int nbytes, nread;
  clock_t startTime, endTime;
  size_t intsize = 4;
  pid_t childpid;

  // set up pipe descriptors and create child process
  pipe(p);
  childpid = fork();

  printf("forked %d\n", childpid);

  // if there was an error forking, send error message and exit function
  if (childpid < 0) {
    perror("Error forking");
    return -1.0;
  }
  
  /* Parent process */
  if(childpid != 0) {
    startTime = clock();
    printf("start: %d\n", (int)startTime);
    // sit in a loop and wait for signal from child so we can send a token
    while (NUMTRIALS > 0) {
      nbytes = read(p[0], &nread, intsize);

      if (nbytes == -1) {     // continue looping if there's nothing in the pipe to read
        break;
      }
      else if (nbytes = 0) {  // exit funtion if connection has closed
        clock_t endTime = clock();
        printf("end: %d\n", (int)endTime);
        return (double)childpid;
      }
      else {            // if child has responded, send another token
        NUMTRIALS -= nread;   // decrement counter
        write(p[1], &one, intsize);
      }
    }
  }

  /* Child process (childpid == 0) */
  else {
    int i=0;

    // send first message to parent
    write(p[1], &one, intsize);

    // sit in a loop and wait for parent to be ready to receive token
    // only send the number of tokens given in NUMTRIALS
    while (i < NUMTRIALS) {
      nbytes = read(p[0], &nread, intsize);

      if (nbytes == -1) {     // continue looping if there's nothing in the pipe to read
        break;
      }
      else if (nbytes = 0) {  // exit function if connection has closed
        return 0;
      }
      else {            // if parent has responded, send another token
        write(p[1], &one, intsize);
        i++;
      }
    }
  }
}

/**********************************************/
void setCPUAffinity(int CPU) {
  cpu_set_t affinityMask;
  CPU_ZERO(&affinityMask);
  CPU_SET(CPU, &affinityMask);
  sched_setaffinity(0, sizeof(&affinityMask), &affinityMask);
}