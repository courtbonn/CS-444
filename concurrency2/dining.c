#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <cpuid.h> 
#include <pthread.h>
#include <semaphore.h>
#include <time.h> 

pthread_t philosopher[5];
pthread_mutex_t forks[5]; 

//https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand
_Bool SupportsRDRAND()
{
	const unsigned int flag_RDRAND = (1 << 30);
	unsigned int eax, ebx, ecx, edx;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	return ((ecx & flag_RDRAND) == flag_RDRAND);
}

int gen_rand(int min, int max)
{
	unsigned int value;
	if(!SupportsRDRAND()){
		value = (rand() % (max + 1 - min)) + min;
	}
	else{
		__asm__("rdrand  %[value]"				   											
			: [value] "=r" (value)
			:       /* no inputs */
			: "cc"  /* clobbers flags (condition codes) */				   													
		);
		value = (value % (max + 1 - min)) + min;																							
	}
	return value;
}

int init_rand()
{
	if(!SupportsRDRAND()){
		init_genrand(time(NULL));
	}
}

void *philos(int i) 
{
	int think = gen_rand(1,20);
	int eat = gen_rand(2,9);

	printf("Philosopher #%d is currently thinking.\n", i); 
	sleep(think); 

	//get_forks()
	pthread_mutex_lock(&forks[i]); // pick up left fork
	pthread_mutex_lock(&forks[(i + 1) % 5]); // pick up right fork

	printf("Philosopher #%d is currently eating.\n", i);
	sleep(eat);

	//put_forks(); 
	pthread_mutex_unlock(&forks[i]); //put down left fork
	pthread_mutex_unlock(&forks[(i + 1) % 5]); // put down right fork
	
	printf("Philosopher #%d is done eating.\n", i);

}


int main() 
{
	time_t t;
	srand((unsigned) time(&t));
	init_rand();
	
	int i;

	for(i = 1; i <= 5; i++) {
		pthread_mutex_init(&forks[i], NULL);
	}

	for(i = 1; i <= 5; i++) {
		pthread_create(&philosopher[i], NULL,(void *) philos,(int *) i);
	}

	for(i = 1; i <= 5; i++) {
		pthread_join(philosopher[i],NULL);
	}

	return 0;
}
