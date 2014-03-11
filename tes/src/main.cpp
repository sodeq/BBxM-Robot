/*
 * main.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: lynx
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN64
   //define something for Windows (64-bit)
#elif _WIN32
   //define something for Windows (32-bit)
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
        // Unsupported platform
    #endif
#elif __linux
    // linux
#elif __unix // all unices not caught above
    // Unix
#elif __posix
    // POSIX
#endif

void GetCPUInfo () {
#ifdef __linux__
	printf("__linux__\n");
#elif __linux
	printf("__linux\n");
#elif _WIN32 || _WIN64
	printf("windows\n");
#endif

	int a, b;
	for(a = 0; a < 5; a++) {
		__asm__("cpuid"
				:"=a"(b)
				:"0"(a)
				:"%ebx", "%ecx", "%edx");
		printf("The code %i gives %#x\n", a, b);
	}
	int i;
	unsigned int index = 0;
	unsigned int regs[4];
	int sum;
	__asm__ __volatile__(
#if defined(__x86_64__) || defined(_M_AMD64) || defined (_M_X64)
		"pushq %%rbx     \n\t" /* save %rbx */
#else
		"pushl %%ebx     \n\t" /* save %ebx */
#endif
		"cpuid            \n\t"
		"movl %%ebx ,%[ebx]  \n\t" /* write the result into output var */
#if defined(__x86_64__) || defined(_M_AMD64) || defined (_M_X64)
		"popq %%rbx \n\t"
#else
		"popl %%ebx \n\t"
#endif
		: "=a"(regs[0]), [ebx] "=r"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
		: "a"(index));

	for (i=4; i<8; i++) {
		printf("%c" ,((char *)regs)[i]);
	}

	for (i=12; i<16; i++) {
		printf("%c" ,((char *)regs)[i]);
	} printf("\n");

	for (i=8; i<12; i++) {
		printf("%c" ,((char *)regs)[i]);
	}printf("\n");
}

struct cobaImg {
	int w,h, nOfParentChannel;
	unsigned char *data;
};

// NOTE: UNTESTED
// TODO: TEST
cobaImg* Split(int width, int height, const unsigned char* in, int nc) {
	if(nc == 1) {
		perror("Input already has single channel\n");
		return NULL;
	}
	cobaImg *spectrum;
	spectrum = (cobaImg *)malloc(sizeof(cobaImg)*nc);
	for(int idx = 0; idx < nc; ++idx) {
		spectrum[idx].data = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
		spectrum[idx].w = width; spectrum[idx].h = height;
		spectrum[idx].nOfParentChannel = nc;
		for(int x = 0; x < width; ++x)
			for(int y = 0; y < height; ++y)
				spectrum[idx].data[(x*width + y)] = in[(y*height + x)*nc + idx];
	}

//	for(int x = 0; x < width ; ++x)
//		for(int y = 0; y < height; ++y)
//			for(int l = 0; l < nc; ++l)
//				spectrum[l].data[(x*width + y)] = in[(y*height + x)*nc +l];

	return spectrum;
}

char *safeMem(char *);
char *unsafeMem(char *);

int main(int n, char** arg) {
	int h = 10, w = 3;
	int *p ;
	p = (int *)malloc(sizeof(int)*h*w);
	for(int i = 0; i < h*w; ++i)
		p[i] = i;

	cobaImg cb, *CB;
	CB = &cb;
	// akses data ke (2,3);
	printf("%i\n", p[0*w + 0]);	// (0,0)
	printf("%i\n", p[1*w + 1]);	// (1,1)
	printf("%i\n", p[2*w + 1]);	// (2,1)
	GetCPUInfo();
	printf("sizeof struct:%i\n", sizeof(cb));
	printf("sizeof struct:%i\n", sizeof(CB));
	return 0;
}

char *safeMem(char *p) {
	char *q = (char *)malloc(strlen(p)+1);
	strcpy(q, p);
	return q;
}
char *unsafeMem(char *p) {
	char q[strlen(p)+1];
	strcpy(q,p);
	return q;
}
