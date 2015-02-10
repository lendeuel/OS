#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <unistd.h>

//#include <fcntl.h> //remove this

/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition -c assign.c 
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort (unsigned long in)
{
    unsigned long arg = in;
    //printf("mid: %0*lx\n", 16, arg);
    int i, j;
    unsigned char * ptr = (unsigned char *)&arg;
    unsigned char min;
    unsigned char * minptr;
    int todo=8;
    /*for(i=0; i<8; i++)
    {
        printf("byte %d - %02x\n", i, ptr[i]);
    }*/
    for(i=0; i<8; i++)
    {
        for(j=0; j<todo; j++)
        {
            if(j==0 || ptr[7-j]<min)
            {
                min=ptr[7-j];
                minptr=&ptr[7-j];
            }
        }
        //printf("found min %02x\n", min);
        todo--;
        minptr[0]=ptr[7-todo];
        ptr[7-todo]=min;
        //printf("mid: %0*lx\n", 16, arg);
    }
    //printf("returning: %0*lx\n", 16, arg);
    return arg;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort (unsigned long arg)
{
    //printf("input: %0*lx\n", 16, arg);
    int i, j;
    unsigned char chars[16];
    unsigned char * ptr = (unsigned char *)&arg;
    unsigned char last = 0xF;
    unsigned char first = 0xF0;
    unsigned char min;
    unsigned char * minptr;
    int todo=16;
    for(i=0; i<8; i++)
    {
        chars[2*i]=(ptr[i]&last);
        chars[2*i+1]=((ptr[i]&first)>>4);
    }
    /*for(i=0; i<16; i++)
    {
        printf("Nibble %d - %x\n", i, chars[i]);
    }*/
    for(i=0; i<16; i++)
    {
        for(j=0; j<todo; j++)
        {
            if(j==0 || chars[j]<min)
            {
                min=chars[j];
                minptr=&chars[j];
            }
        }
        //printf("found min %02x\n", min);
        todo--;
        minptr[0]=chars[todo];
        chars[todo]=min;
        //printf("mid: %0*lx\n", 16, arg);
    }
    /*for(i=0; i<16; i++)
    {
        printf("Nibble %d - %x\n", i, chars[i]);
    }*/
    unsigned long out = 0;
    for(i=0; i<16; i++)
    {
        out = out<<4;
        out = out | chars[i];
    }
    //printf("returning: %0*lx\n", 16, out);
    return out;
}

/*********************************************************************
 *
 * little_linked_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *  
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

struct elt *name_list (void)
{
    //int d;
    int i;
    //struct elt* base = NULL;
    struct elt* base = (struct elt*)malloc(sizeof(struct elt));
    /*printf("Allocing D\n");
    const char name[6] = "DEUEL";*/
    struct elt* next = base;
    for(i=0; i<4; i++)
    {
        if(next == NULL)
        {
            //d=0;
            while(base!=NULL)
            {
                //printf("Freeing %c\n", name[d]);
                //d++;
                next=base->link;
                free(base);
                base=next;
            }
            return NULL;
        }
        else
        {
            //printf("Allocing %c\n", name[i+1]);
            /*if(i==0)
            {
                next->link=NULL;
                printf("Allocing %c failed\n", name[i+1]);
            }*/
            //else
            //{
                next->link=(struct elt*)malloc(sizeof(struct elt));
            //}
            next=next->link;
        }
    }
    if(next == NULL)
    {
        //d=0;
        while(base!=NULL)
        {
            //printf("Freeing %c\n", name[d]);
            //d++;
            next=base->link;
            free(base);
            base=next;
        }
        return NULL;
    }
    base->val='D';
    base->link->val='E';
    base->link->link->val='U';
    base->link->link->link->val='E';
    base->link->link->link->link->val='L';
    /*next=base;
    while(next!=NULL)
    {
        printf("character %c\n", next->val);
        next=next->link;
    }*/
    return base;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert (enum format_t mode, unsigned long value)
{
    int i;
    if(mode==HEX)
    {
        for(i=0; i<16; i++)
        {
            unsigned short test = (value>>(60-4*i))&0xf;
            if(test<10)
                putc('0'+test, stdout);
            else
                putc('a'+(test-10), stdout);
        }
        putc('\n', stdout);
    }
    if(mode==OCT)
    {
        if(value>>63)
            putc('1', stdout);
        else
            putc('0', stdout);
        for(i=0; i<21; i++)
        {
            unsigned short test = (value>>(60-3*i))&0x7;
            putc('0'+test, stdout);
        }
        putc('\n', stdout);
    }
    if(mode==BIN)
    {
        for(i=0; i<64; i++)
        {
            unsigned short test = (value>>(63-i))&0x1;
            putc('0'+test, stdout);
        }
        putc('\n', stdout);
    }
    
}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 * 
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me (void)
{
    
    int r;
    unsigned short O_WRONLY=0x1;
    unsigned short O_CREAT=0x40;
    
    int fd = syscall(0x2, "me.txt", O_WRONLY | O_CREAT, 0x1FF);
    if(fd==-1)
    {
        return;
    }
    char outbuf[]="    \\()/   \n"
                  "     ||    \n"
                  "     /\\    \n";
    r=syscall(0x1, fd, outbuf, 36);
    if(r==-1)
    {
        syscall(87, "me.txt");
        return;
    }
    r=syscall(0x3, fd);
    if(r==-1)
    {
        syscall(87, "me.txt");
        //printf("Close failed.\n");
    }
}
