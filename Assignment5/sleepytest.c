#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
     char byte;
     int fd = open("/dev/sleepy0", O_RDWR);
     write(fd, 10, 1);
     ssize_t size = read(fd, &byte, 1);
     printf("Read byte %c\n", byte);
     return 0;
}