#include <stdio.h>
#include "assign1.c"

void binaryToHex()
{
    int i;
    char binary[]="1101111010101101101111101110111111011110101011011011111011101111";
    unsigned long total=0;
    for(i=0; i<64; i++)
    {
        total*=2;
        total+=(binary[i]-'0');
    }
    printf("bin to hex %0*lx\n", 16, total);
}

void octalToHex()
{
    int i;
    char octal[]="1572555756773653337357";
    unsigned long total=0;
    for(i=0; i<22; i++)
    {
        total*=8;
        total+=(octal[i]-'0');
    }
    printf("oct to hex %0*lx\n", 16, total);
}

int main(int argc, const char* argv[])
{
    draw_me();
}
