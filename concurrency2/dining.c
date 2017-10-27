#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <cpuid.h> 
#include <pthread.h>
#include <semaphore.h>
#include <time.h> 
#include <string.h>

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

void name(char s[]){
	if(strcmp(s,"Philosopher #1") == 0){
		printf("Plato");
	}
	else if(strcmp(s,"Philosopher #2") == 0){
	   	printf("Aristotle");
	}
	else if(strcmp(s,"Philosopher #3") == 0){
	   	printf("Descartes");
	}
	else if(strcmp(s,"Philosopher #4") == 0){
	   	printf("Kant");
	}
	else if(strcmp(s,"Philosopher #5") == 0){
	   	printf("Socrates");
	}
}

void *philos(int i) 
{
	int think = gen_rand(1,20);
	int eat = gen_rand(2,9);
	
	char buffer[24];
	sprintf(buffer, "Philosopher #%d", i);
	
	name(buffer);
	printf(" is currently thinking.\n");
	sleep(think); 
	
	//get_forks()
	pthread_mutex_lock(&forks[i]); // pick up left fork
	pthread_mutex_lock(&forks[(i + 1) % 5]); // pick up right fork
	printf("Forks around ");
	name(buffer);
	printf(" are being used.\n");

	name(buffer);
	printf(" is currently eating.\n");
	sleep(eat);

	//put_forks(); 
	pthread_mutex_unlock(&forks[i]); //put down left fork
	pthread_mutex_unlock(&forks[(i + 1) % 5]); // put down right fork
	printf("Forks around ");
	name(buffer);
	printf(" have just been set down.\n");

	name(buffer);
	printf(" is done eating.\n");

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
