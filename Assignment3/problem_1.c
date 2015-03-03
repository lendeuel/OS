//
//  problem_1.c
//  
//
//  Created by Len Deuel on 3/1/15.
//
//

#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _thread_data_t
{
    int tid;
} thread_data_t;

volatile int in_cs=0;
volatile int* tickets;
volatile int* entering;
volatile int* cs_count;
int NUM_THREADS;
int NUM_SECONDS;

void lock(int tid)
{
    entering[tid]=1;
    int m = 0;
    int i = 0;
    for(i=0; i<NUM_THREADS; i++)
    {
        if(tickets[i]>m)
        {
            m=tickets[i];
        }
    }
    tickets[tid]=m+1;
    entering[tid]=0;
    for(i=0; i<NUM_THREADS; ++i)
    {
        if(i!=tid)
        {
            while(entering[i])
            {
            }
            
            while(tickets[i]!=0 && (tickets[tid]>tickets[i] || (tickets[tid]==tickets[i] && tid>i)))
            {
            }
        }
    }
}

void critical_section()
{
    assert (in_cs==0);
    in_cs++;
    assert (in_cs==1);
    in_cs++;
    assert (in_cs==2);
    in_cs++;
    assert (in_cs==3);
    in_cs=0;
}

void unlock(int tid)
{
    tickets[tid]=0;
}


void* start(void* arg)
{
    int tid = (*((thread_data_t* )arg)).tid;
    while(1)
    {
        lock(tid);
        cs_count[tid]++;
        critical_section();
        unlock(tid);
    }
}


int main(int argc, char* argv[])
{
    if(argc<3)
    {
        printf("Invalid number of arguements\n");
        return -1;
    }
    NUM_THREADS = atoi(argv[1]);
    NUM_SECONDS = atoi(argv[2]);
    if(NUM_THREADS<1 || NUM_SECONDS <1)
    {
        printf("Invalid arguements.\n");
        return -1;
    }
    thread_data_t data[NUM_THREADS];
    entering = malloc(NUM_THREADS * sizeof(int));
    tickets = malloc(NUM_THREADS * sizeof(int));
    cs_count = malloc(NUM_THREADS * sizeof(int));
    pthread_t thread_arr[NUM_THREADS];
    int i;
    for(i=0; i < NUM_THREADS; i++)
    {
        tickets[i]=0;
        entering[i]=0;
        cs_count[i]=0;
    }
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
    
    if(sleep(NUM_SECONDS)>0)
    {
        printf("Error while sleeping, did not sleep full amount.\n");
        return -1;
    }
    //switch print method
    for(i=0; i < NUM_THREADS; i++)
    {
        printf("Thread number %i entered the critical section %i times.\n", i, cs_count[i]);
    }
    return 0;
}

