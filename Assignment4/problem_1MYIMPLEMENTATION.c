//
//  problem_1.c
//
//  Created by Len Deuel on 3/25/15.
//  Copyright (c) 2015 lendeuel. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

typedef enum
{
    bird, dog, cat
    
} animal;

typedef struct thread_info
{
    int tid;
    animal animal_type;
    int count;
    pthread_t thread;
    pthread_mutex_t* lock;
    int signaled;
    
} thread_info;

typedef struct thread_queue
{
    volatile struct thread_info* thread;
    volatile struct thread_queue* next;
    
} thread_queue;

volatile int go=1;
volatile int cats=0;
volatile int dogs=0;
volatile int birds=0;
volatile int NUM_CATS=0;
volatile int NUM_DOGS=0;
volatile int NUM_BIRDS=0;
volatile thread_queue* bird_queue=NULL;
volatile thread_queue* dog_queue=NULL;
volatile thread_queue* cat_queue=NULL;

sigset_t fSigSet;

void monitor_wait(thread_info* info)
{
    //printf("wait\n");
    thread_queue newEntry;
    newEntry.thread = info;
    newEntry.next = NULL;
    if(info->animal_type == bird)
    {
        volatile thread_queue* cursor = bird_queue;
        if(cursor==NULL)
        {
            bird_queue=&newEntry;
        }
        else
        {
            while(cursor->next != NULL)
            {
                cursor = cursor->next;
            }
            cursor->next = &newEntry;
        }
    }
    
    if(info->animal_type == cat)
    {
        volatile thread_queue* cursor = cat_queue;
        if(cursor==NULL)
        {
            cat_queue=&newEntry;
        }
        else
        {
            while(cursor->next != NULL)
            {
                cursor = cursor->next;
            }
            cursor->next = &newEntry;
        }
    }
    
    if(info->animal_type == dog)
    {
        volatile thread_queue* cursor = dog_queue;
        if(cursor==NULL)
        {
            //printf("starting a new dog queue");
            dog_queue=&newEntry;
        }
        else
        {
            while(cursor->next != NULL)
            {
                cursor = cursor->next;
            }
            cursor->next = &newEntry;
        }
    }
    
    pthread_mutex_unlock(info->lock);
    
    if(!info->signaled)
    {
        int nSig;
        sigwait(&fSigSet, &nSig);
    }
    
    pthread_mutex_lock(info->lock);
    info->signaled=0;
}

void cat_enter(thread_info* info)
{
    //printf("cat enter\n");
    pthread_mutex_lock(info->lock);
    //printf("lock acquired");
    while(birds!=0 || dogs!=0)
    {
        monitor_wait(info);
    }
    cats++;
    info->count++;
    //printf("lock released");
    pthread_mutex_unlock(info->lock);
}

void bird_enter(thread_info* info)
{
    //printf("bird enter\n");
    pthread_mutex_lock(info->lock);
    while(cats!=0)
    {
        monitor_wait(info);
    }
    birds++;
    info->count++;
    pthread_mutex_unlock(info->lock);
}

void dog_enter(thread_info* info)
{
    //printf("dog enter\n");
    pthread_mutex_lock(info->lock);
    while(cats!=0)
    {
        monitor_wait(info);
    }
    dogs++;
    info->count++;
    pthread_mutex_unlock(info->lock);
}

void cat_exit(thread_info* info)
{
    //printf("cat exit\n");
    pthread_mutex_lock(info->lock);
    //printf("lock acquired\n");
    cats--;
    if(cats==0)
    {
        volatile thread_queue* cursor = bird_queue;
        bird_queue=NULL;
        while(cursor != NULL)
        {
            cursor->thread->signaled=1;
            pthread_kill((cursor->thread->thread), SIGUSR1);
            cursor = cursor->next;
        }
        cursor = dog_queue;
        dog_queue=NULL;
        while(cursor != NULL)
        {
            cursor->thread->signaled=1;
            pthread_kill((cursor->thread->thread), SIGUSR1);
            cursor = cursor->next;
        }
    }
    //printf("lock released\n");
    pthread_mutex_unlock(info->lock);
}

void bird_exit(thread_info* info)
{
    //printf("bird exit\n");
    pthread_mutex_lock(info->lock);
    birds--;
    if(birds==0 && dogs==0)
    {
        volatile thread_queue* cursor = cat_queue;
        cat_queue=NULL;
        while(cursor != NULL)
        {
            cursor->thread->signaled=1;
            pthread_kill((cursor->thread->thread), SIGUSR1);
            cursor = cursor->next;
        }
    }
    pthread_mutex_unlock(info->lock);
}

void dog_exit(thread_info* info)
{
    //printf("dog exit\n");
    pthread_mutex_lock(info->lock);
    dogs--;
    if(birds==0 && dogs==0)
    {
        volatile thread_queue* cursor = cat_queue;
        cat_queue=NULL;
        while(cursor != NULL)
        {
            cursor->thread->signaled=1;
            pthread_kill((cursor->thread->thread), SIGUSR1);
            cursor = cursor->next;
        }
    }
    pthread_mutex_unlock(info->lock);
}

void play()
{
    for (int i=0; i<10; i++)
    {
        //printf("checking legal cat count\n");
        assert(cats >= 0 && cats <= NUM_CATS);
        //printf("checking legal dog count\n");
        assert(dogs >= 0 && dogs <= NUM_DOGS);
        //printf("checking legal bird count\n");
        assert(birds >= 0 && birds <= NUM_BIRDS);
        //printf("checking no cats + dogs\n");
        assert(cats == 0 || dogs == 0);
        //printf("checking no cats + birds\n");
        assert(cats == 0 || birds == 0);
    }
}

void* start(void* arg)
{
    thread_info* info = ((thread_info* )arg);
    
    if(info->animal_type == bird)
    {
        while(go)
        {
            bird_enter(info);
            play();
            bird_exit(info);
        }
    }
    
    if(info->animal_type == cat)
    {
        while(go)
        {
            cat_enter(info);
            play();
            cat_exit(info);
        }
    }
    
    if(info->animal_type == dog)
    {
        while(go)
        {
            dog_enter(info);
            play();
            dog_exit(info);
        }
    }
    
    return NULL;
}

void sigusr_handler(int sig)
{
    return;
}

int main(int argc, char* argv[])
{
    sigemptyset(&fSigSet);
    sigaddset(&fSigSet, SIGUSR1);
    sigaddset(&fSigSet, SIGSEGV);
    
    struct sigaction action, old_action;
    action.sa_handler = sigusr_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    
    if (sigaction(SIGUSR1, &action, &old_action) < 0)
    {
        printf("sigaction failed\n");
    }
    
    if(argc<4)
    {
        printf("Invalid number of arguements\n");
        return -1;
    }
    
    NUM_CATS = atoi(argv[1]);
    NUM_DOGS = atoi(argv[2]);
    NUM_BIRDS = atoi(argv[3]);

    if(NUM_CATS<0 || NUM_BIRDS <0 || NUM_DOGS <0)
    {
        printf("Invalid arguements.\n");
        return -1;
    }
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    thread_info data[NUM_CATS+NUM_DOGS+NUM_BIRDS];
    int i;
    
    for(i=0; i < NUM_CATS+NUM_DOGS+NUM_BIRDS; i++)
    {
        //printf("making thread %i\n", i);
        data[i].signaled=0;
        data[i].tid = i;
        data[i].lock = &lock;
        if(i<NUM_CATS)
        {
            data[i].animal_type = cat;
        }
        else if(i<(NUM_DOGS+NUM_CATS))
        {
            data[i].animal_type = dog;
        }
        else
        {
            data[i].animal_type = bird;
        }
        data[i].count = 0;
        int e = pthread_create(&(data[i].thread), NULL, start, &data[i]);
        if(e)
        {
            printf("Failed to create threads\n");
            return -1;
        }
    }
    
    if(sleep(10)>0)
    {
        printf("Error while sleeping, did not sleep full amount.\n");
        return -1;
    }
    go=0;
    {
        for(i=0; i < NUM_CATS+NUM_DOGS+NUM_BIRDS; i++)
        {
            pthread_join(data[i].thread, NULL);
        }
    }
    for(i=0; i < NUM_CATS+NUM_DOGS+NUM_BIRDS; i++)
    {
        if(i<NUM_CATS)
        {
            printf("A cat (thread number %i) entered the critical section %i times.\n", i, data[i].count);
        }
        else if(i<(NUM_DOGS+NUM_CATS))
        {
            printf("A dog (thread number %i) entered the critical section %i times.\n", i, data[i].count);
        }
        else
        {
            printf("A bird (thread number %i) entered the critical section %i times.\n", i, data[i].count);
        }
    }
    return 1;
}


