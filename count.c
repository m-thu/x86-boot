#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KERNEL_SIZE 50

int main()
{
	struct stat s;
	unsigned int seg, add;

	stat("kernel.bin", &s);
	seg = (unsigned int)s.st_size / 512;

	if (((unsigned int)s.st_size % 512) != 0) {
		add = (seg + 1) * 512 - (unsigned int)s.st_size;
		++seg;
	} else {
		add = 0;
	}

	printf("%i\n", (KERNEL_SIZE - seg) * 512 + add);
	return 0;
}
