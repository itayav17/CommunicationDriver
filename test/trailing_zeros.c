#include <stdio.h>



void main()
{
    unsigned int bits = 0;
    unsigned int x = 16;

    // mask the 16 low order bits, add 16 and shift them out if they are all 0.
    if (!(x & 0xFFFF))
    {
        bits = 16;
        x >>= 16;
    }

    // mask the 8 low order bits, add 8 and shift them out if they are all 0.
    if (!(x & 0xFF))
    {
        bits +=  8;
        x >>=  8;
    }

    // mask the 4 low order bits, add 4 and shift them out if they are all 0.
    if (!(x & 0xF))
    {
        bits +=  4;
        x >>=  4;
    }

    // mask the 2 low order bits, add 2 and shift them out if they are all 0.
    if (!(x & 0x3))
    {
        bits +=  2;
        x >>=  2;
    }

    // mask the low order bit and add 1 if it is 0.
    bits += (x & 1) ^ 1;
    
    printf("Trailing lowest bits:%u\n",bits);

    bits = 0;
    x = 16;

    // mask the 16 low order bits, add 16 and shift them out if they are all 0.
    if (!(x & 0xFFFF0000))
    {
        bits = 16;
        x <<= 16;
    }

    // mask the 8 low order bits, add 8 and shift them out if they are all 0.
    if (!(x & 0xFF000000))
    {
        bits +=  8;
        x <<=  8;
    }

    // mask the 4 low order bits, add 4 and shift them out if they are all 0.
    if (!(x & 0xF0000000))
    {
        bits +=  4;
        x <<=  4;
    }

    // mask the 2 low order bits, add 2 and shift them out if they are all 0.
    if (!(x & 0xc0000000))
    {
        bits +=  2;
        x <<=  2;
    }

    // mask the low order bit and add 1 if it is 0.
    bits += (x & 0x1) ^ 1;

    printf("Trailing lowest bits:%u\n",bits);
}
