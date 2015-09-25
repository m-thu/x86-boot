#include "print.h"

void cpuid()
{
	unsigned int eax, ebx, ecx, edx, eflags;
	unsigned int buf[3*4+1];
	int i;

	asm volatile("pushf \n\t"
		"popl %0 \n\t"
		"orl $(1 << 21), %0 \n\t"		/* ID flag */
		"pushl %0 \n\t"
		"popf \n\t"
		"pushf \n\t"
		"popl %0"
		: "=r" (eflags));

	if (eflags & (1 << 21)) {
		eax = 0;
		asm volatile("cpuid"
			: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
			: "a" (eax));
		buf[0] = ebx;
		buf[1] = edx;
		buf[2] = ecx;
		buf[3] = 0;
		printf("cpuid, vendor id: %s\n", (char *)buf);

		eax = 0x80000000;
		asm volatile("cpuid"
			: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
			: "a" (eax));
		if (eax >= 0x80000004) {
			for (i = 0; i < 3; ++i) {
				eax = 0x80000002 + i;
				asm volatile("cpuid"
					: "=a" (eax), "=b" (ebx), "=c" (ecx),
					  "=d" (edx)
					: "a" (eax));
				buf[i*4] = eax;
				buf[i*4+1] = ebx;
				buf[i*4+2] = ecx;
				buf[i*4+3] = edx;
			}
			buf[3*4] = 0;
			printf("cpuid, brand string: %s\n", (char *)buf);
		} else {
			print("cpuid brand string not supported!\n");
		}
	} else {
		print("cpuid instruction not supported!\n");
	}
}
