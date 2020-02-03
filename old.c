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

  printf("forked %d\n", childpid);

  // if there was an error forking, send error message and exit function
  if (childpid < 0) {
    perror("Error forking");
    return -1;
  }
  
  /* Parent process */
  if(childpid != 0) {
    startTime = clock();
    printf("start: %d\n", childpid);
    // sit in a loop and wait for signal from child so we can send a token
    while (NUMTRIALS > 0) {
      nbytes = read(p[0], &nread, intsize);
      printf("%d:\t%d\n", childpid, nbytes);

      if (nbytes == -1) {     // continue looping if there's nothing in the pipe to read
       printf("\tempty\n");
        break;
      }
      else if (nbytes = 0) {  // exit funtion if connection has closed
        clock_t endTime = clock()/CLOCKS_PER_SEC;
        printf("\tend: %d\n", (int)endTime);
        close(p[0]); close(p[1]);
        // kill(childpid, SIGKILL);
        return -1;
      }
      else {            // if child has responded, send another token
        printf("\tWRITETO  %d\n", childpid);
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
        printf("CHILD ENDING\n");
        close(p[0]); close(p[1]);
        return -1;
      }
      else {            // if parent has responded, send another token
        printf("\tWRITING BACK\n");
        write(p[1], &one, intsize);
        i++;
      }
    }
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