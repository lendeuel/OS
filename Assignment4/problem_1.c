//
//  problem_1.c
//  MoviePaint
//
//  Created by Len Deuel on 3/25/15.
//  Copyright (c) 2015 lendeuel. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum
{
    bird, dog, cat
    
} animal;

typedef struct _spin_lock_t
{
    int value;
} spin_lock_t;

typedef struct
{
    int tid;
    animal a;
    int count;
    pthread_t* thread;
    spin_lock_t* lock;
    
} thread_info;

typedef struct
{
    struct thread_info* thread;
    struct thread_queue* next;
    
} thread_queue;

volatile int cat_playing_count=0;
volatile int dog_playing_count=0;
volatile int bird_playing_count=0;

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

void spin_unlock(spin_lock_t* s)
{
    //printf("exiting lock\n");
    atomic_cmpxchg(&(s->value), 1, 0);
}

void cat_enter()
{
}

void bird_enter()
{
}

void dog_enter()
{
}

void cat_exit()
{
}

void bird_exit()
{
}

void dog_exit()
{
}

int play()
{
    return 1;
}


int main(int argc, char* argv[])
{
    return 1;
}


