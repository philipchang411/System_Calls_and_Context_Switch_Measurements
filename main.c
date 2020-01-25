#include <stdio.h>
#include <time.h>

double arrayIteration();

int main()
{
    clock_t begin = clock();
    double arrayTime;
    arrayTime= arrayIteration();
    clock_t end = clock();
    double total_time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Total time: %f ms \n", arrayTime);
}

double arrayIteration()
{
    FILE *file;
    file = fopen("Record.txt", "w");
    fprintf(file, "In Array Iteration: \n");
    int storage[1000000];

    clock_t begin = clock();
    int size;
    size = sizeof(storage)/sizeof(storage[0]);
    for(int i = 0; i < size; i++)
    {
        //printf("WRITING NOW\n");
        storage[i] = i;
        if(i%10000 == 0)
        {
            clock_t readingTimeStart = clock();
            for(int j = 0; j < i; j++)
            {
                int temp;
                temp = storage[j]; 
            }
            clock_t readingTimeEnd = clock();
            double readingTime = (double)(readingTimeEnd - readingTimeStart) / CLOCKS_PER_SEC;
            fprintf(file, "Time needed to read current array of size %i : %f ms \n", i, readingTime);
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    fclose(file);
    return time_spent;
}