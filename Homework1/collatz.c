//
//  collatz.c
//
//  Created by Len Deuel on 1/24/15.
//

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 5
int nums[SIZE] = {0,1,2,3,4};
int main(int argc, const char* argv[])
{
    int i = atoi(argv[1]);
    if(i<=0)
    {
        printf("input must be a postive integer\n");
    }
    pid_t pid;
    pid = fork();
    if(pid<0)
    {
        printf("failed to create child process\n");
    }
    if (pid == 0)
    {
        printf("%d", i);
        while(i!=1)
        {
            if(i%2==0)
            {
                i/=2;
            }
            else
            {
                i*=3;
                i++;
            }
            printf(", %d", i);
        }
        printf("\n");
    }
    else
    {
        wait(NULL);
    }
    return 0;
}