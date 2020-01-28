#include <stdio.h>
#include <time.h>

double arrayIteration();

int main()
{
    //Begins the clock to get the total run time of the program
    //clock_t begin = clock();
    double arrayTime;
    arrayTime= arrayIteration();
    //clock_t end = clock();
    //double total_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Total time: %f ms \n", arrayTime);
}

double arrayIteration()
{
    //Opens the file that the results are printed to in order to see the increase in time that a larger structure takes to navigate
    FILE *file, *times;
    file = fopen("System_Call.txt", "w");
    times = fopen("TimeRecord.txt", "w");
    fprintf(file, "Array Iteration: \n");
    int storage[1000000];

    //Starts the clock when the reading and writing begins 
    clock_t begin = clock();
    int size;
    size = sizeof(storage)/sizeof(storage[0]);

    int totalInterations = 0;
    double sumTime = 0;
    //for loop writes to the array and fills it in 
    for(int i = 0; i < size; i++)
    {
        //writing
        //storage[i] = i;
        fprintf(file, "%i\n", i);
        if(i%10000 == 0)
        {
            //only counds the time when it starts reading
            clock_t readingTimeStart = clock();
            for(int j = 0; j < i; j++)
            {
                //reading portion
                int temp=0;
                fscanf(file, "%d", &temp); 
            }
            clock_t readingTimeEnd = clock();
            double readingTime = (double)(readingTimeEnd - readingTimeStart) / CLOCKS_PER_SEC;
            sumTime = sumTime + readingTime;
            fprintf(times, "Time needed to read current array of size %i : %f ms \n", i, readingTime);
            totalInterations++;
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    fclose(file);
    double averageTime = sumTime/(float)totalInterations;
    fprintf(times, "\n\nAverage Time: %f ms\n", averageTime);
    fclose(times);
    return time_spent;
}