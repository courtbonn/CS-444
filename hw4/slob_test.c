#include <sys/syscall.h>
#include <stdio.h>
#include <linux/kernel.h>

int main(){
   	long int used = syscall(359);
	long int free = syscall(360);

	printf("Used SLOB sys call: %ld\n", used);
	printf("Free SLOB sys call: %ld\n", free);

	return 0;
}
