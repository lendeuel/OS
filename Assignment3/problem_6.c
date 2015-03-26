//
//  problem_6.c
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
#include <time.h>
#include <math.h>

typedef struct _spin_lock_t
{
    int value;
} spin_lock_t;

typedef struct _thread_data_t
{
    int tid;
    spin_lock_t* lock;
} thread_data_t;

volatile int* cs_count;
int NUM_SECONDS;
int NUM_THREADS;
volatile int POINTS_CREATED=0;
volatile int POINTS_IN_CIRCLE=0;

double getValue()
{
    return ((2*rand())/(double)RAND_MAX)-1;
}

void mfence ()
{
    asm volatile ("mfence" : : : "memory");
}

static inline int atomic_cmpxchg (volatile int *ptr, int old, int new)
{
    int ret;
    asm volatile ("lock cmpxchgl %2,%1"
                  : "=a" (ret), "+m" (*ptr)
                  : "r" (new), "0" (old)
                  : "memory");
    return ret;
}

void spin_lock(spin_lock_t* s)
{
    //printf("entering lock\n");
    while(atomic_cmpxchg(&(s->value), 0, 1))
    {
        if(sched_yield()<0)
        {
            printf("sched_yield failed.\n");
            exit(-1);
        }
    }
    //printf("acquired lock\n");
}

void critical_section(int in_circle)
{
    if(in_circle)
    {
        POINTS_IN_CIRCLE++;
    }
    POINTS_CREATED++;
}

void spin_unlock(spin_lock_t* s)
{
    //printf("exiting lock\n");
    atomic_cmpxchg(&(s->value), 1, 0);
}


void* start(void* arg)
{
    int tid = (*((thread_data_t* )arg)).tid;
    spin_lock_t* lock = (*((thread_data_t* )arg)).lock;
    while(1)
    {
        int x = getValue();
        int y = getValue();
        int in_circle = (int)(sqrt(x*x+y*y)<=1);
        
        spin_lock(lock);
        cs_count[tid]++;
        critical_section(in_circle);
        spin_unlock(lock);
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
    cs_count = malloc(NUM_THREADS * sizeof(int));
    pthread_t thread_arr[NUM_THREADS];
    spin_lock_t lock;
    lock.value=0;
    int i;
    for(i=0; i < NUM_THREADS; i++)
    {
        cs_count[i]=0;
    }
    for(i=0; i < NUM_THREADS; i++)
    {
        data[i].tid = i;
        data[i].lock = &lock;
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
    for(i=0; i < NUM_THREADS; i++)
    {
        printf("Thread number %i entered the critical section %i times.\n", i, cs_count[i]);
    }
    
    double pi = (4*POINTS_IN_CIRCLE)/(double)POINTS_CREATED;
    printf("estimating pi as %f\n", pi);
    printf("points in circle: %i\n", POINTS_IN_CIRCLE);
    printf("points: %i\n", POINTS_CREATED);
    return 0;
}

