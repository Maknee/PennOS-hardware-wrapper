#include "string.h"

int strcmp(const char* str1, const char* str2)
{
	int result = 0;
	while (!(result = *str1 - *str2) && *str1)
	{
		str1++;
		str2++;
	}

	return result;
}

int strncmp(const char* str1, const char* str2, size_t count)
{
	int result = 0;
	while (*str1 && *str2 && !(result = *str1 - *str2) && count)
	{
		str1++;
		str2++;
		count--;
	}

	return result;
}

size_t strlen(const char* str)
{
	size_t length = 0;
	while (*str != '\0')
	{
		length++;
		str++;
	}
	return length;
}

char* strncpy (char * dst, const char * src, size_t count)
{
	for(size_t i = 0; i < count && src[i]; i++)
	{
		dst[i] = src[i];
	}
	return dst;
}

//http://clc-wiki.net/wiki/C_standard_library:string.h:strncat
char* strncat(char* dst, const char* src, size_t count)
{
	char *ret = dst;
	while (*dst)
		dst++;
	while (count--)
		if (!(*dst++ = *src++))
			return ret;
	*dst = 0;
	return ret;
}

//https://articles.leetcode.com/implement-strstr-to-find-substring-in/

char* strstr(const char *str, const char *target)
{
	if (!*target) return (char*)str;
	char *p1 = (char*)str, *p2 = (char*)target;
	char *p1Adv = (char*)str;
	while (*++p2)
		p1Adv++;
	while (*p1Adv) {
		char *p1Begin = p1;
		p2 = (char*)target;
		while (*p1 && *p2 && *p1 == *p2) {
			p1++;
			p2++;
		}
		if (!*p2)
			return p1Begin;
		p1 = p1Begin + 1;
		p1Adv++;
	}
	return NULL;
}

void* memcpy(void* dst, const void* src, size_t count)
{
	for(size_t i = 0; i < count; i++)
	{
		((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
	}
	return dst;
}

//why is mine wrong?
/*
void* memmove(void* dst, const void* src, size_t count)
{

	if((uintptr_t)dst < (uintptr_t)src)
	{
		//copy forward
		for (size_t i = 0; i < count; i++)
		{
			((uint8_t*)dst)[i] = ((uint8_t*)src)[i];
		}
	}
	else
	{
		//copy backwards
		for (count -= 1; count >= 0; count--)
		{
			((uint8_t*)dst)[count] = ((uint8_t*)src)[count];
		}

	}
	return dst;
}
*/

void *memmove(void *dest, void const *src, size_t n)
{
	register char *dp = dest;
	register char const *sp = src;
	if(dp < sp) {
		while(n-- > 0)
			*dp++ = *sp++;
	} else {
		dp += n;
		sp += n;
		while(n-- > 0)
			*--dp = *--sp;
	}

	return dest;
}

void* memset(void* dst, uint8_t val, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		((uint8_t*)dst)[i] = val;
	}
	return dst;
}

int memcmp(const void* dst, const void* src, size_t count)
{
	for(size_t i = 0; i < count; i++)
	{
		if(((uint8_t*)dst)[i] == ((uint8_t*)src)[i])
		{
			return ((uint8_t*)dst)[i] - ((uint8_t*)src)[i];
		}
	}
	return 0;
}


