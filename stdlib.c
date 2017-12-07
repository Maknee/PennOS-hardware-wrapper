#include "stdlib.h"

void itoa_u(uint32_t i, char* buf, uint32_t base)
{
    if (i == 0)
    {
        *buf = '0';
        return;
    }

    // each character is placed into an array from which we will index
    char numbers[] = "0123456789ABCDEF";

    // when we iterate through the numbers, it will be flipped
    char copyBuf[32] = { 0 };

    // check for 0's
    if (i == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
    }

    uint32_t pos = 0;

    // iterate through copy buffer and grab the string values
    while (i != 0)
    {
        copyBuf[pos++] = numbers[i % base];
        i /= base;
    }

    // null terminate the buffer
    buf[pos] = '\0';

    // start at last element
    uint32_t lastIndex = pos--;

    // since the copy buffer is flipped, we copy the reverse into buffer
    for (uint32_t j = 0; j < lastIndex; j++, pos--)
    {
        buf[j] = copyBuf[pos];
    }
}

void itoa(int32_t i, char* buf, uint32_t base)
{
    // check if the base is > 16 (doesn't make sense for greater)
    if (base > 16)
    {
        return;
    }

    // if i is negative, put - sign
    if (i < 0)
    {
        *buf++ = '-';
        i *= -1;
    }
    itoa_u((uint32_t)i, buf, base);
}

int atoi(const char* str)
{
	int result = 0;
	for(; *str; str++)
	{
		result = result * 10 + *str - '0';
	}
	return result;
}

void exit(int status)
{
	printf("OS exited with status %d\nYou can yank out the power cord now :)\n", status);
	abort();
}


