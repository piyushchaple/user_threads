#ifndef QUEUE_H
#define QUEUE_H
#include "thread.h"

// structure for job queue
typedef struct queue {
	queue_node *head;
	queue_node *tail;
}queue;

/// functions for queue
int enqueue(queue *q, queue_node *temp);
queue_node* dequeue(queue *q);
int init_queue(queue *q);

#endif