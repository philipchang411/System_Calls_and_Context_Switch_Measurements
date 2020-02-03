#define _GNU_SOURCE

#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/*** FUNCTIONS ***/
double doSyscall(int numcalls, int file);

/*** MAIN ***/
int main() {

  // Variables used to calculate averages
  int i;
  int numcalls = 1000;    // Starting number of iterations; increases as program progresses
  int numtrials = 10;
  double timesum = 0.0;

  // Open file
  int fd = open("dummy.txt", O_RDONLY | O_CREAT, 0222);

  // Starting message
  printf("\n**** SYSTEM CALLS ****\nTrial\t# calls\tTrial Time\tAverage Time for Sys Call\n");

  // Run trials multiple times
  for (i=0; i<numtrials; i++) {
    // Increase number of system calls by a factor of 10 after every 5th trial
    if(i%5 == 0)
      numcalls *= 10;
    
    // Run the determined amount of syscalls
    double t = doSyscall(numcalls, fd);
    if (t > 0) {
      // If time recorded is "good" (nonnegative), record the trial time
      timesum += t/numcalls;
      printf("%d\t%d\t%f ms\t%f ms\n", i+1, numcalls, t, t/numcalls);
    }
    else {
      // If the time recorded is "bad" (negative), throw out the trial results and repeat
      if (i%5 == 0)
        numcalls /= 10;
      i--;
    }
  }

  // Close and remove file
  close(fd);
  remove("dummy.txt");

  // Final message and average
  printf("Overall average time per system call: %f ms\n\n", timesum/numtrials);

  return 0;
}

/*** FUNCTIONS ***/
double doSyscall(int numcalls, int file) {
  int *buf;
  int i;
  struct timespec startTime, endTime;

  // Records starting time
  clock_gettime(CLOCK_REALTIME, &startTime);

  // Perform 0 byte reads (numcall) times
  for (i=0; i<numcalls; i++) {
    read(file, buf, 0);
  }

  // Record ending time and return difference from starting time
  clock_gettime(CLOCK_REALTIME, &endTime);
  double t = ((endTime.tv_nsec-startTime.tv_nsec)*1.0)/(1.0e6);

  return t;
}