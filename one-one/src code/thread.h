#ifndef THREADS_H
#define THREADS_H
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h> 
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <asm-generic/errno.h>
#include <stdbool.h> 
#include <stdatomic.h> 
#include <setjmp.h>
#include <errno.h>

// size of each thread stack
#define THREAD_STACK 64 * 1024

// for initializing maximum number of threads
#define MAX_THREADS 40

// user level thread sturcture
typedef struct thread_t {
	int tid;  
	int pid;  
}thread_t;

typedef struct thread_user{
	pid_t pid; 
	int tid; 
	void *stack; 
	struct thread_user *next, *prev; 
	void*				args;
	void*				(*start) (void *);	
	void *retrive_values;	
	bool retrive_flag;  
}thread_user;

// structure of  spinlock
typedef struct thread_spinlock_t {
	bool flag;	// checking  whether lock is available or not
}thread_spinlock_t;

void init();
int gettidfrompid(int pid);
// create new Thread control block
thread_user* newNode();
int killing_thread(thread_t thread, int signo);
void exit_from_thread(void *retrive_values);
bool validthreadid(int tid);
thread_user* getNodeUsingPid(int pid);
void print();
int creating_thread(thread_t *thread, void *(*routine_start) (void *), void *arg);
bool anotherThreadAlreadyWaitingToJoinWithThisThread(int waitertid);
int joining_thread(thread_t thread, void **np);
// add a job to job queue 
void add(thread_user *p);
// remove  node from queue 
thread_user* noderemoving(int tid);
int execution_of_function(void *p);

#endif