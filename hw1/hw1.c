#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <cpuid.h>
#include <pthread.h>

_Bool SupportsRDRAND; //true if supported, false if not supported
pthread_t prod;
pthread_t cons;

//TODO: some type of queue structure for the event buffer

struct buffer_vals{
	int num;
	int rand_time;
	};


//https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand
_Bool SupportsRDRAND(){
	const unsigned int flag_RDRAND = (1 << 30);
	unsigned int eax, ebx, ecx, edx;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	return ((ecx & flag_RDRAND) == flag_RDRAND);
}

int init_rand(){
	supports_rdrand = SupportsRDRAND();
	if(!supports_rdrand){
		init_genrand(time(NULL));
	}
}

void *producer(void *param){
	//init a new event
	//fill event (with random number (srand function in stdlib))
	//see if the queue is full
	//see if the queue is busy (waiting time)
	//put the event into the buffer with random wait time
	//increment the semaphore	
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
	
	return 0;
}

