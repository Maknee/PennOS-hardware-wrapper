#include "unistd.h"

#define FLATFS_FD 0xdeadbeef

static uint8_t* start_fs_cursor;
static uint8_t* fs_cursor;

int open(const char* name, int flags)
{
	//reset cursor
	fs_cursor = start_fs_cursor;
	return FLATFS_FD;
}

ssize_t read(int fd, void *buf, size_t count)
{
	if(fd == STDIN_FILENO)
	{
		//read from keyboard
		if(!keyboard_read_buffer(buf, count))
		{
			return 0;
		}
		else
		{
			return strlen(buf);
		}
	}
	else if(fd == FLATFS_FD)
	{
		//read from memory
		memcpy(buf, fs_cursor, count);

		//update cursor
		fs_cursor += count;
	}
	return (ssize_t)count;
}

ssize_t write(int fd, const void *buf, size_t count)
{
	if(fd == STDOUT_FILENO)
	{
		for(size_t i = 0; i < count; i++)
		{
			vga_putc(((char*)buf)[i]);
		}
	}
	else if(fd == FLATFS_FD)
	{
		//write to memory
		memcpy(fs_cursor, buf, count);

		//update cursor
		fs_cursor += count;
	}
	return (ssize_t)count;
}

int lseek(int fd, off_t offset, int whence)
{
	if(fd == FLATFS_FD)
	{
		if(whence == SEEK_SET)
		{
			//update cursor
			fs_cursor = start_fs_cursor + offset;
		}
	}
	return offset;
}

int close(int fd)
{
	if(fd == FLATFS_FD)
	{
		//update cursor
		fs_cursor = start_fs_cursor;
	}
	return 0;
}

void init_unistd_lib(void* cursor)
{
	start_fs_cursor = cursor;
	fs_cursor = cursor;
}

void* get_fs_cursor()
{
	return start_fs_cursor;
}



