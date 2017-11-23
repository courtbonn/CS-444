#include <stdio.h>
#include <stdlib.h>
#include "mt19937ar.c"
#include <pthread.h>
#include <time.h>
#include <string.h>

//blocks for when the list is being acted on
//might not need the search mutex??
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
		count = count->next;
	}
	return count;
}

//searchers merely examine the list
void search(){
	struct node *item = head;
	while(1){
		pthread_mutex_lock(&search_mutex);
		while(item != NULL) {
			printf("item is %d\n", item->data);
			item = item->next;
		}

		pthread_mutex_unlock(&search_mutex);
		sleep(1);
	}
}

//inserters add new items to the end of the list
void insert(){
	//new is the element to be inserted
	struct node *new;
	while(1){
	   	pthread_mutex_lock(&insert_mutex);
		curr = head;

	   	if(count() < 10){
			//make node to insert
			new = (struct node *)malloc(sizeof(struct node));
			new->data = rand();
			new->next = NULL;

			//find end of list
			while(curr->next != NULL)
			   	curr = curr->next;
			
			//insert new node
			curr->next = new;

			pthread_mutex_unlock(&insert_mutex);
			sleep(1);
		}
	}
}

//deleters remove items from anywhere in the list
void delete(){
	struct node *item, *front, *back;
	while(1){
		pthread_mutex_lock(&search_mutex);
		pthread_mutex_lock(&insert_mutex); 

		//remove the head
		if(head == NULL) {
			print("List is empty; Nothing to delete\n");
			return NULL;
		} else {
			front = head;
			head = head->next;
			front->next = NULL;
			if(front == head) 
				head = NULL;
			free(front);
			return head; 
		}

		
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
	new_node->data = rand();
	head = new_node;
	head->next = NULL;
	
	//create threads
	for(i = 0; i < 3; i++){
	   	pthread_create(&searcher[i], NULL, search, NULL);
		pthread_create(&inserter[i], NULL, insert, NULL);
		pthread_create(&deleter[i], NULL, delete, NULL);
	}

	//join threads
	for(i = 0; i < 3; i++){
	   	pthread_join(inserter[i], NULL);
		pthread_join(searcher[i], NULL);
		pthread_join(deleter[i], NULL);
	}


	return 0;
}
