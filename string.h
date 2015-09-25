/*#include "string.h"*/

static __attribute__((unused))
void *memcpy(void *dest, const void *src, unsigned int n)
{
	void *tmp = dest;
	unsigned int i0, i1, i2;

	asm volatile("cld" ::: "memory");
	if (n % 4) {
		asm volatile("rep movsb"
			: "=&S" (i0), "=&D" (i1), "=&c" (i2)
			: "0" (src), "1" (dest), "2" (n));
	} else {
		asm volatile("rep movsl"
			: "=&S" (i0), "=&D" (i1), "=&c" (i2)
			: "0" (src), "1" (dest), "2" (n/4));
	}

	return tmp;
}

static __attribute__((unused))
void *memset(void *s, int c, unsigned int n)
{
	void *tmp = s;
	unsigned int i0, i1, c2;

	asm volatile("cld" ::: "memory");
	if (n % 4) {
		asm volatile("rep stosb"
			: "=&c" (i0), "=&D" (i1)
			: "0" (n), "1" (s), "a" (c));
	} else {
		c2 = (unsigned char)c | ((unsigned char)c << 8)
			| ((unsigned char)c << 16)
			| ((unsigned char)c << 24);
		asm volatile("rep stosl"
			: "=&c" (i0), "=&D" (i1)
			: "0" (n/4), "1" (s), "a" (c2));
	}

	return tmp;
}

static __attribute__((unused))
char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;
	unsigned int i0, i1;

	asm volatile("cld \n\t"
		"1: lodsb \n\t"
		"stosb \n\t"
		"testb %%al, %%al \n\t"
		"jnz 1b"
		: "=&S" (i0), "=&D" (i1)
		: "0" (src), "1" (dest)
		: "%al");

	return tmp;
}

static __attribute__((unused))
unsigned int strlen(const char *s)
{
	unsigned int count, i0;

	asm volatile ("cld \n\t"
		"repne scasb"
		: "=&D" (i0), "=&c" (count)
		: "0" (s), "a" (0), "1" (0xffffffff));

	return 0xffffffff - count - 1;
}
