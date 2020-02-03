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
int contextSwitch(int NUMTRIALS) {
  int p[2];
  int one = 1;
  int nbytes, nread;
  clock_t startTime, endTime;
  size_t intsize = 4;
  pid_t childpid;

  // set up pipe descriptors and create child process
  pipe(p);
  childpid = fork();

  // if there was an error forking, send error message and exit function
  if (childpid < 0) {
    perror("Error forking");
    return -1;
  }

  if(childpid != 0) {
    startTime = clock();
    write(p[1], &one, intsize);
    close(p[0]); close(p[1]);
    return (int)startTime;
  }
  else {
    nbytes = read(p[0], &nread, intsize);
    close(p[0]); close(p[1]);
    return nbytes;
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