#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

//blocks for when the list is being acted on
pthread_mutex_t search_mutex;
pthread_mutex_t delete_mutex;
pthread_mutex_t insert_mutex;

sem_t delete_insert_sem;
sem_t delete_search_sem;

//linked list
//http://www.zentut.com/c-tutorial/c-linked-list/
struct node{
	int data;
	struct node* next;
} *head;

//print list
void print_list(){
	struct node *curr;
	int i;
	curr = head;
	printf("CURRENT LIST: ");
	while(curr != NULL){
	   	printf("[%d] ", curr->data);
	   	curr = curr->next;
	}
	printf("\n");
}

//we'll need a method to check the size of the list
//insert when list < max size
//delete when list > 1
int count(){
	struct node *temp;
	int count = 0;
	temp = head;
	while(temp != NULL){
		count++;
		temp = temp->next;
	}
	return count;
}

//searchers merely examine the list
//multiple searchers are allowed, no lock
void *searcher(){
	struct node *item;
	while(1){
		//wait for delete to finish
		sem_wait(&delete_search_sem);

		item = head;
		if (count() == 0){
		   	printf("SEARCH: list is empty\n");
		//	pthread_mutex_unlock(&delete_mutex);
			sleep(1);
			break;
		} else {
		printf("SEARCHER: ");
		while(item != NULL) {
			printf("[%d] ", item->data);
			item = item->next;
		}
		printf("\n");
		}
		sleep(1);
	}
}

//inserters add new items to the end of the list
void *inserter(){
	//new is the element to be inserted
	struct node *new, *curr;
	while(1){
	   	if(count() < 15) {
		//wait for delete to finish
	   	sem_wait(&delete_insert_sem);
		//just one inserter allowed
	   	//pthread_mutex_lock(&insert_mutex);

		//make node to insert
		new = (struct node *)malloc(sizeof(struct node));
		new->data = rand() % 50;
		new->next = NULL;
		printf("INSERTER: [%d]\n", new->data);	

		//somehow there is no list to insert into
		//init linked list
		if (head == NULL){
			head = new;
			head->next = NULL;
			printf("INSERTER: new node: [%d]\n", head->data);
		}else{
			//find end of list
			curr = head;
			while(curr->next != NULL)
		   		curr = curr->next;
			//insert new node
			curr->next = new;
		}
		print_list();
		//pthread_mutex_unlock(&insert_mutex);
		sleep(1);
		}
	} 
}

//deleters remove items from anywhere in the list
//let's try to delete a random index
void *deleter(){
	struct node *item, *prev;
	int i, rand_idx;
	while(1){
	//	pthread_mutex_lock(&search_mutex);
	//	pthread_mutex_lock(&insert_mutex);
		item = head;
		
		//get a random index within the size of the list
		if (head == NULL){
		   	printf("DELETER: list is already empty\n");
			break;
		} else {
			rand_idx = rand() % count();	
			
			printf("DELETER: count=%d rand_idx=%d\n", count(), rand_idx);
			//set item to the list element
			for (i = 0; i < rand_idx; i++){
				prev = item;
				item = item->next;
			}
			//delete first element
			if (item == head){
				head = head->next;
				item->next = NULL;
				free(item);
			}
			//delete last element
			else if (item->next == NULL){
				prev->next = NULL;
				free(item);
				print_list();
			}
			//delete element from middle
			else{ 
				if (item != NULL){
					prev->next = item->next;
					item->next = NULL;
					free(item);
				}
			}
		}
		print_list();

		//this break should escape the loop correctly
		break;
                                                
	}
	//pthread_mutex_unlock(&search_mutex);
	//pthread_mutex_unlock(&insert_mutex); 

	//wake up the search and insert if they were waiting
	sem_post(&delete_insert_sem);
	sem_post(&delete_search_sem);
	sleep(1);	
}

int main(){
	int i;
	srand(time(NULL));
	
	pthread_mutex_init(&search_mutex, NULL);
	pthread_mutex_init(&insert_mutex, NULL);
	pthread_mutex_init(&delete_mutex, NULL);

	sem_init(&delete_insert_sem, 0, 1);
	sem_init(&delete_search_sem, 0, 1);

	pthread_t searcher_th[3];
	pthread_t inserter_th[3];
	pthread_t deleter_th[3];

	//create threads
	for(i = 0; i < 3; i++){
		pthread_create(&inserter_th[i], NULL, (void *)inserter, (void *)NULL);
		pthread_create(&searcher_th[i], NULL, (void *)searcher, (void *)NULL);
		pthread_create(&deleter_th[i], NULL, (void *)deleter, (void *)NULL);
	}

	//join threads
	for(i = 0; i < 3; i++){
	   	pthread_join(inserter_th[i], NULL);
		pthread_join(searcher_th[i], NULL);
		pthread_join(deleter_th[i], NULL);
	}
	return 0;
}
