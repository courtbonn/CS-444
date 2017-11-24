#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <time.h>
#include <string.h>

//blocks for when the list is being acted on
pthread_mutex_t search_mutex;
pthread_mutex_t delete_mutex;
pthread_mutex_t insert_mutex;

//linked list
//http://www.zentut.com/c-tutorial/c-linked-list/
struct node{
	int data;
	struct node* next;
} *head;

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
void *search(){
	struct node *item = head;
	while(1){
		pthread_mutex_lock(&search_mutex);
		if (count() == 0){
		   	printf("SEARCH: list is empty\n");
			pthread_mutex_unlock(&search_mutex);
			sleep(1);
			break;
		}
		while(item != NULL) {
			printf("[%d] ", item->data);
			item = item->next;
		}
		printf("\n");

		pthread_mutex_unlock(&search_mutex);
		sleep(1);
	}
}

//inserters add new items to the end of the list
void *insert(){
	//new is the element to be inserted
	struct node *new, *curr;
	while(1){
	   	pthread_mutex_lock(&insert_mutex);
		pthread_mutex_lock(&delete_mutex);
		curr = head;
			
		//make node to insert
		new = (struct node *)malloc(sizeof(struct node));
		new->data = rand() % 50;
		new->next = NULL;
		
		//find end of list
		while(curr->next != NULL)
		   	curr = curr->next;
		
		//insert new node
		curr->next = new;
		
		pthread_mutex_unlock(&insert_mutex);
		pthread_mutex_unlock(&delete_mutex);
		sleep(6);
	}
}

//deleters remove items from anywhere in the list
//let's try to delete a random index
void *delete(){
	struct node *item, *prev;
	int i, rand_idx;
	while(1){
		pthread_mutex_lock(&search_mutex);
		pthread_mutex_lock(&insert_mutex); 
		pthread_mutex_lock(&delete_mutex);
		item = head;
		
		//get a random index within the size of the list
		if (count() == 0){
		   	printf("DELETER: list is already empty\n");
			break;
		}
		rand_idx = rand() % count();	
			
		printf("DELETER: count=%d rand_idx=%d\n", count(), rand_idx);
		//set item to the list element
		for (i = 0; i < rand_idx; i++){
			prev = item;
			item = item->next;
		}
		//delete first element
		if (item == head){ 
		   	//empty list
		   	if (head == NULL)
			   	break;
			head = item->next;
			free(item);
		}
		//delete last element
		else if (item->next == NULL){
		   	//empty list
		  	if (head == NULL)
			  	break; 
			prev->next = NULL;
			free(item);
		}
		//delete element from middle
		else{ 
		   	//empty list
			if (head == NULL)
			   	break;
			if (item != NULL){
				prev->next = item->next;
				free(item);
			}
		}
		
		pthread_mutex_unlock(&search_mutex);
		pthread_mutex_unlock(&insert_mutex);
		pthread_mutex_unlock(&delete_mutex);
		sleep(2);
		
	}
	//need this to unlock after break
	pthread_mutex_unlock(&search_mutex);
	pthread_mutex_unlock(&insert_mutex);
	pthread_mutex_unlock(&delete_mutex);
	sleep(2);	
}

int main(){
	int i;
	srand(time(NULL));
	
	pthread_t searcher[3];
	pthread_t inserter[3];
	pthread_t deleter[3];

	//init linked list
	struct node *new_node;
	new_node = (struct node *)malloc(sizeof(struct node));
	new_node->data = rand() % 50;
	head = new_node;
	head->next = NULL;
	
	//create threads
	for(i = 0; i < 3; i++){
	   	pthread_create(&searcher[i], NULL, (void *)search, NULL);
		pthread_create(&inserter[i], NULL, (void *)insert, NULL);
		pthread_create(&deleter[i], NULL, (void *)delete, NULL);
	}

	//join threads
	for(i = 0; i < 3; i++){
	   	pthread_join(inserter[i], NULL);
		pthread_join(searcher[i], NULL);
		pthread_join(deleter[i], NULL);
	}
	return 0;
}
