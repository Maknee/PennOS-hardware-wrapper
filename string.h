#pragma once

#include "includes.h"

int strcmp(const char* str1, const char* str2);

int strncmp(const char* str1, const char* str2, size_t count);

char* strncat(char* dst, const char* src, size_t count);

char* strstr(const char *str, const char *target);

size_t strlen(const char* str);

char* strncpy (char * dst, const char * src, size_t count);

void* memcpy(void* dst, const void* src, size_t count);

void* memmove(void* dst, const void* src, size_t count);

void* memset(void* dst, uint8_t val, size_t size);

int memcmp(const void* dst, const void* src, size_t count);



