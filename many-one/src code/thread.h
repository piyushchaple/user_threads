#ifndef THREAD_H
#define THREAD_H

#include<assert.h>
#include<setjmp.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/time.h>
#include<sys/resource.h>
#include <stdatomic.h> 
#include<errno.h>

#define THREAD_STACK (1024*64)
#define MAX_THREADS 50

typedef struct thread_t {
	int tid; 
}thread_t;

// thread control block
typedef struct queue_node {
	int tid; 
	jmp_buf context; 
	void *(*routine_start) (void *); 
	void *arg; 
	int active; 
	void *stack; 
	void *retrive_values;  
	bool retrive_flag; 		
	
	struct queue_node *next;	
	int signo; 
}queue_node;

void scheduler();
// used to set timer
static bool init_timer();
//  creating  new thread
int creating_thread(thread_t *thread, void *(*routine_start) (void *), void *arg);
int joining_thread(thread_t thread, void **np);
void execute(int signum);
void init_main_context();
int exit_from_thread(void *retrive_values);

#endif