//
//  problem_1.c
//  
//
//  Created by Len Deuel on 3/25/15.
//
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

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
} thread_info;

volatile int go=1;
volatile int cats=0;
volatile int dogs=0;
volatile int birds=0;
volatile int NUM_CATS=0;
volatile int NUM_DOGS=0;
volatile int NUM_BIRDS=0;
pthread_cond_t* bird_cv;
pthread_cond_t* cat_cv;
pthread_cond_t* dog_cv;

void cat_enter(thread_info* info)
{
    //printf("cat enter\n");
    pthread_mutex_lock(info->lock);
    //printf("lock acquired");
    while(birds!=0 || dogs!=0)
    {
        pthread_cond_wait(cat_cv, info->lock);
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
        pthread_cond_wait(bird_cv, info->lock);
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
        pthread_cond_wait(dog_cv, info->lock);
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
        pthread_cond_broadcast(bird_cv);
        pthread_cond_broadcast(dog_cv);
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
        pthread_cond_broadcast(cat_cv);
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
        pthread_cond_broadcast(cat_cv);
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

int main(int argc, char* argv[])
{
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
    
    pthread_cond_t bird_local;
    pthread_cond_t dog_local;
    pthread_cond_t cat_local;
    
    dog_cv=&dog_local;
    cat_cv=&cat_local;
    bird_cv=&bird_local;

    
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_init (bird_cv, NULL);
    pthread_cond_init (dog_cv, NULL);
    pthread_cond_init (cat_cv, NULL);
    thread_info data[NUM_CATS+NUM_DOGS+NUM_BIRDS];
    int i;
    
    for(i=0; i < NUM_CATS+NUM_DOGS+NUM_BIRDS; i++)
    {
        //printf("making thread %i\n", i);
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
    for(i=0; i < NUM_CATS+NUM_DOGS+NUM_BIRDS; i++)
    {
        pthread_join(data[i].thread, NULL);
    }
    int cat_total=0;
    int bird_total=0;
    int dog_total=0;
    for(i=0; i < NUM_CATS+NUM_DOGS+NUM_BIRDS; i++)
    {
        if(i<NUM_CATS)
        {
            cat_total+=data[i].count;
        }
        else if(i<(NUM_DOGS+NUM_CATS))
        {
            dog_total+=data[i].count;
        }
        else
        {
            bird_total+=data[i].count;
        }
    }
    printf("cat played = %i, dog played = %i, bird played = %i\n", cat_total, dog_total, bird_total);
    return 1;
}

