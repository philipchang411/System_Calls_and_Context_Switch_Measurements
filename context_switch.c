#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

/*** Function Declarations ***/
int contextSwitch();
double estimateSetupCost();
void setCPUAffinity();
void setup();

/*** Main ***/
int main() {

  // Set the machine to s single processor
  setCPUAffinity(3);

  int trials = 5;
  int time = 0;

  // Measure cost of a context switch
  time = contextSwitch(trials);
  printf("MAIN  %d\n", time);

  // double est = estimateSetupCost();
  // printf("%f\n", est);

  return 0;
}

/*** Function Definitions ***/
/**********************************************/
int contextSwitch(int numtrials) {
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
    clock_t startTime, endTime;
    startTime = clock();

    while (num > 0) {
      nbytes = read(p[0], &nread, intsize);

      if (nbytes == -1) {     // continue looping if there's nothing in the pipe to read
        continue;
      }
      else if (nbytes == 0) {  // exit funtion if connection has closed
        break;
      }
      else {            // if child has responded, send another token
        num -= nread;   // decrement counter
        write(p[1], &one, intsize);
      }
    }

    endTime = clock()/CLOCKS_PER_SEC; // record end time

    close(p[0]); close(p[1]);   // close pipe
    kill(childpid, SIGKILL);    // kill child so its value is not passed up to the main process

    return num;
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
      else if (nbytes == 0) {  // exit function if connection has closed
        break;
      }
      else {            // if parent has responded, send another token
        write(p[1], &one, intsize);
        i++;
      }
    }

    // close the pipe and return (parent function should kill child before child returns this value)
    close(p[0]); close(p[1]);
    return -1;
  }
}

/**********************************************/
double estimateSetupCost() {
  clock_t start, end;
  start = clock();

  setup();
  end = clock();
  return (double)(end-start)/CLOCKS_PER_SEC;
}

/**********************************************/
void setCPUAffinity(int CPU) {
  cpu_set_t affinityMask;
  CPU_ZERO(&affinityMask);
  CPU_SET(CPU, &affinityMask);
  sched_setaffinity(0, sizeof(&affinityMask), &affinityMask);
}

void setup() {
  int p[2];
  int one = 1;
  int nbytes, nread;
  clock_t startTime, endTime;
  size_t intsize = 4;
  pid_t childpid;

  // set up pipe descriptors and create child process
  pipe(p);
  childpid = fork();
  kill(childpid,SIGKILL);

  printf("Process %d\n", childpid);
  return;
}