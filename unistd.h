#pragma once

#include "includes.h"
#include "vga.h"
#include "keyboard.h"

//linux custom type
typedef int off_t;
typedef int ssize_t;

#define O_RDWR 1
#define O_CREAT 2

int open(const char* name, int flags);

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

ssize_t read(int fd, void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count);

enum 
{
	SEEK_SET,
	SEEK_CUR,
	SEEK_END
};

int lseek(int fd, off_t offset, int whence);

int close(int fd);

//not part of library...
void init_unistd_lib(void* cursor);

void* get_fs_cursor();

