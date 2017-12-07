#pragma once

#include "unistd.h"

//filler
enum
{
	PROT_READ = 0x1,
	PROT_WRITE = 0x2
};

enum
{
	MAP_SHARED = 0x1
};

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

int munmap(void *addr, size_t length);

