#include "mman.h"

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	return get_fs_cursor();
}

int munmap(void *addr, size_t length)
{
	return 0;
}
