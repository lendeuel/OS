//
//  problem_6.c
//  
//
//  Created by Len Deuel on 3/3/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

typedef struct _thread_data_t
{
    int tid;
} thread_data_t;

int NUM_POINTS=1000;
int NUM_THREADS=1;
volatile int POINTS_CREATED=0;
volatile int POINTS_IN_CIRCLE=0;

double getValue()
{
    return ((2*rand())/(double)RAND_MAX)-1;
}

void* start(void* arg)
{
    int tid = (*((thread_data_t* )arg)).tid;
    
    int x = getValue();
    int y = getValue();
    int in_circle = (int)(sqrt(x*x+y*y)<=1);
    //lock here
    if(in_circle)
    {
        POINTS_IN_CIRCLE++;
    }
    POINTS_CREATED++;
    //unlock here
}

int main()
{
    int i;
    srand((int)time(NULL));
    
    thread_data_t data[NUM_THREADS];
    pthread_t thread_arr[NUM_THREADS];
    
    for(i=0; i < NUM_THREADS; i++)
    {
        data[i].tid = i;
        int e = pthread_create(&thread_arr[i], NULL, start, &data[i]);
        if(e)
        {
            printf("Failed to create threads\n");
            return -1;
        }
    }
    
    while(POINTS_CREATED<NUM_POINTS)
    {
    }
    
    double pi = (4*POINTS_IN_CIRCLE)/POINTS_CREATED;
    printf("estimating pi as %f", pi);
    
    return 0;
}
