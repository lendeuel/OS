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

typedef struct _spin_lock_t
{
    int value;
} spin_lock_t;

typedef struct _thread_data_t
{
    int tid;
    spin_lock_t* lock;
} thread_data_t;

volatile int in_cs=0;
volatile int* cs_count;
int NUM_THREADS;
int NUM_SECONDS;

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
  while(atomic_cmpxchg(&(s->value), 0, 1))
  {
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

void spin_unlock(spin_lock_t* s)
{
    atomic_cmpxchg(&(s->value), 1, 0);
}


void* start(void* arg)
{
    int tid = (*((thread_data_t* )arg)).tid;
    spin_lock_t* lock = (*((thread_data_t* )arg)).lock;
    while(1)
    {
        spin_lock(lock);
        cs_count[tid]++;
        critical_section();
        spin_unlock(lock);
    }
}


int main(int argc, char* argv[])
{
    if(argc<3)
    {
        //error
        return -1;
    }
    NUM_THREADS = atoi(argv[1]);
    NUM_SECONDS = atoi(argv[2]);
    thread_data_t data[NUM_THREADS];
    cs_count = malloc(NUM_THREADS * sizeof(int));
    pthread_t thread_arr[NUM_THREADS];
    spin_lock_t lock;
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
            //error
            return -1;
        }
    }
    
    if(sleep(NUM_SECONDS)>0)
    {
        //error
        return -1;
    }
    
    for(i=0; i < NUM_THREADS; i++)
    {
        printf("Thread number %i entered the critical section %i times.\n", i, cs_count[i]);
    }
    return 0;
}

