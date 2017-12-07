#pragma once
#include "includes.h"
#include "vga.h"
#include "string.h"

//Credits go to https://www.youtube.com/watch?v=BeSpPd3C3J8

typedef struct MemoryChunk
{
	struct MemoryChunk *next;
	struct MemoryChunk *prev;
	bool allocated;
	size_t size;
} MemoryChunk;

void init_mm(size_t start, size_t size);
void* malloc(size_t size);
void free(void*);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

