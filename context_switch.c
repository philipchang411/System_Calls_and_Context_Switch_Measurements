#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/*** Function Declarations ***/
double contextSwitch(int numtrials);
double runTrial(int numtrials);
void setCPUAffinity();

/*** Main ***/
int main() {

  // Set the machine to s single processor
  setCPUAffinity(3);

  // Variables to calculate averages
  int trials = 10000;
  double count = 0.0;
  double timesum = 0.0;

  // Starting message
  printf("**** CONTEXT SWITCH ****\nTrial\t# switches\tTrial Time\tAverage Time for Context Switch\n");

  // Run trials multiple times
  for (int i=0; i<10; i++) {
    if (i == 5)
      trials *= 10;
    double t = contextSwitch(trials);
    // Sometimes the gettime method returns bogus times which result in negative numbers; disregard these trials
    if (t > 0) {
      timesum += t;
      printf("%d\t%d\t\t%f ms\t%f ms\n", (int)++count, trials, t, t/trials);
    }
    else {
      i--;
    }
  }

  // Final message and average
  printf("Average time for context switch overall: %f ms\n", timesum/trials/count);

  return 0;
}

/*** Function Definitions ***/
/**********************************************/
double contextSwitch(int numtrials) {
  int p[2];
  int one = 1;
  int num = numtrials;
  int nbytes, nread;
  size_t intsize = 4;
  pid_t childpid;

  // set up pipe descriptors and create child process
  pipe(p);
  childpid = fork();

  /* Parent process */
  if(childpid != 0) {
    struct timespec startTime, endTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    while (num > 0) {
      nbytes = read(p[0], &nread, intsize);

      if (nbytes == -1) {     // continue looping if there's nothing in the pipe to read
        continue;
      }
      else if (nbytes == 0) {  // if connection has closed, exit loop
        break;
      }
      else {            // if child has responded, send another token
        num -= one;   // decrement counter
        write(p[1], &one, intsize);
      }
    }

    clock_gettime(CLOCK_REALTIME, &endTime); // record end time
    double t = ((endTime.tv_nsec-startTime.tv_nsec)*1.0)/(1.0e6);  // calculate elapsed time

    // printf("%d - %d = %f\n", (int)endTime.tv_sec, (int)startTime.tv_sec, (endTime.tv_nsec-startTime.tv_nsec)*1.0);
    // kill(9, childpid);       // kill child process
    close(p[0]); close(p[1]);   // close pipe

    return t;
  }

  /* Child process */
  else {
    int i=0;

    // send first message to parent
    write(p[1], &one, intsize);

    // sit in a loop and wait for parent to be ready to receive token
    // only send the number of tokens given in numtrials
    while (i < numtrials) {
      nbytes = read(p[0], &nread, intsize);

      if (nbytes == -1) {     // continue looping if there's nothing in the pipe to read
        continue;
      }
      else if (nbytes == 0) {  // if connection has closed, exit loop
        break;
      }
      else {            // if parent has responded, send another token
        write(p[1], &one, intsize);
        i++;        // increment counter
      }
    }

    // close the pipe and return
    close(p[0]); close(p[1]);
    exit(0);
  }
}

/**********************************************/
void setCPUAffinity(int CPU) {
  cpu_set_t affinityMask;
  CPU_ZERO(&affinityMask);
  CPU_SET(CPU, &affinityMask);
  sched_setaffinity(0, sizeof(&affinityMask), &affinityMask);
}