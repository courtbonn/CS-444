#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <cpuid.h>

_Bool SupportsRDRAND; //true if supported, false if not supported

//https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand
_Bool SupportsRDRAND(){
	const unsigned int flag_RDRAND = (1 << 30);
	unsigned int eax, ebx, ecx, edx;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	return ((ecx & flag_RDRAND) == flag_RDRAND);
}

struct buffer_vals{
	int num;
	int rand_time;
};

int main(){
	return 0;
}

