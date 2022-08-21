#include "queue.h"

//double headed queue
int init_queue(queue *q) {
	q->head = NULL;
	q->tail = NULL;
	return 0;
}

// dequeue front element from queue
queue_node* dequeue(queue *q) {
	queue_node *temp;
	int flag = 0;
	if(q->tail == NULL)
		return NULL;
	else {
		if(q->head == q->tail)
			flag = 1;
		temp = q->head;
		q->head = temp->next;
		q->tail->next = q->head;
		temp->next = NULL;
		if(flag) {
			q->head = NULL;
			q->tail = NULL;
		}
		return temp;
	}
}

// adding element to the  queue at end
int enqueue(queue *q, queue_node *temp) {
	if(q->tail == NULL) {
		q->head = temp;
		q->tail = temp;
		temp->next = temp;
	}
	else {
		q->tail->next = temp;
		temp->next = q->head;
		q->tail = temp;			
	}
	return 0;
}