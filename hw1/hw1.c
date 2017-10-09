#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <cpuid.h>
#include <pthread.h>

_Bool SupportsRDRAND; //true if supported, false if not supported
pthread_t prod;
pthread_t cons;

//init rand num gen
srand((unsigned) time(&t));

//blocks changes being made to buffer
pthread_mutex_t busy_mutex;

//blocks consumers if the buffer is empty
pthread_mutex_t empty_mutex;

//blocks producers if the buffer is full
pthread_mutex_t full_mutex;

struct buffer_vals{
	int num;
	int rand_time;
	};

struct buffer_vals buffer[32];
pthread_mutex_t mutex;

//https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand
_Bool SupportsRDRAND(){
	const unsigned int flag_RDRAND = (1 << 30);
	unsigned int eax, ebx, ecx, edx;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	return ((ecx & flag_RDRAND) == flag_RDRAND);
}

int gen_rand(int min, int max){
   	unsigned int value;
	if(!supports_rdrand){
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

int init_rand(){
	supports_rdrand = SupportsRDRAND();
	if(!supports_rdrand){
		init_genrand(time(NULL));
	}
}

void *producer(void *param){
	//init a new event
	struct buffer_vals *new_val;

	//fill event (with random number (srand function in stdlib))
	new_val = malloc(sizeof(struct buffer));
	new_val->num = gen_rand(0,5);
	new_val->rand_time = gen_rand(2,9);	
	
	//see if the queue is full (update state and wait for state for state to change)
	
	//see if the queue is busy (waiting time, update state?)
	//put the event into the buffer with random wait time (3-7)
	int wait = gen_rand(3, 7);
}

void *consumer(void *param){
	//remove an item from buffer
	//init a new event
	//event = queue.head
	//remove event from queue
	//increment semaphore to free the space in the queue
	//consume the event with the random wait time
}

int main(){
   	init_rand();

	//http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/
	//placeholders
	pthread_create(&prod, NULL, *producer, NULL);
	pthread_create(&cons, NULL, *consumer, NULL);
}

