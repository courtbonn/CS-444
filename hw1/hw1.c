#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <cpuid.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

pthread_t prod;
pthread_t cons;
sem_t full;
sem_t busy;
time_t t;

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

int init_rand(){
	if(!SupportsRDRAND()){
		init_genrand(time(NULL));
	}
}

void *producer(void *param){
   	int num;
   	while(1){
		//init a new event
		struct buffer_vals *new_val;

		//fill event (with random number (srand function in stdlib))
		new_val = malloc(sizeof(struct buffer_vals));
		new_val->num = gen_rand(0,5);
		new_val->rand_time = gen_rand(2,9);	
	
		//see if the queue is full (update state and wait for state for state to change)
		sem_wait(&full);

		//see if the queue is busy (waiting time, update state?)
		sem_getvalue(&busy, &num);

		//put the event into the buffer with random wait time (3-7)
		pthread_mutex_lock(&mutex);
		int wait = gen_rand(3, 7);
		sleep(wait);
		buffer[num] = *new_val;
		printf("New event produced and added to buffer");
	}
}

void *consumer(void *param){
   	int num;
	struct buffer_vals val;
	while(1){
		//remove an item from buffer
		sem_wait(&busy);
		pthread_mutex_lock(&mutex);

		//get event
		sem_getvalue(&busy, &num);
		val = buffer[num];

		//increment semaphore to free the space in the queue
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
		
		//consume the event with the random wait time
		sleep(val.rand_time);
		printf("Event consumed from buffer");
	}
}

int main(){
   	srand((unsigned) time(&t));
   	init_rand();

	pthread_mutex_init(&mutex, NULL);
	sem_init(&busy, 0, 0);
	sem_init(&full, 0, 32);

	//http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/
	//placeholders
	pthread_create(&prod, NULL, producer, NULL);
	pthread_create(&cons, NULL, consumer, NULL);

	pthread_join(prod,NULL);
	pthread_join(cons,NULL);

	return 0;
}


