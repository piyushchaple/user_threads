#include "threads.h"
#include "queue.h"
#include "sigalarm.h"

static int counting_thread = 0;
queue_node *current;
queue_node *setter;
queue ready;
queue completed;
int sync_flag = 1;
jmp_buf buf[MAX_THREADS];

// initialize main context
void init_main_context() {
	//Creating node	
	queue_node *main;
	main = (queue_node*)malloc(sizeof(queue_node));	
	main->tid = counting_thread;
	main->routine_start = NULL;
	main->arg = NULL;
	main->active = 1;	
	main->next = NULL;
	main->stack = NULL;
	
	/*Backing up the main context*/
	if (setjmp(main->context)){
		unblocking_sigalaram();
		return;
	}
	current = main;
	// main thread will always be the first thread
	counting_thread++;
	return;
}

int exit_from_thread(void *retrive_values){
	blocking_sigalaram();
	current -> retrive_values = retrive_values;
	longjmp(buf[current -> tid], 1);
	unblocking_sigalaram();
}

int creating_thread(thread_t *thread, void *(*routine_start) (void *), void *arg){
    blocking_sigalaram();
    //Initializing everything if first thread is exist
	if(counting_thread == 0) {		
		init_queue(&ready);
		init_queue(&completed);		
		init_main_context();
		init_timer();
	}

    //Create a node
	queue_node *temp;
	temp = (queue_node*)malloc(sizeof(queue_node));	

	temp->tid = counting_thread;
	temp->routine_start = routine_start;
	temp->arg = arg;
	temp->active = 1;	
	temp->next = NULL;
	temp -> signo = -1;
	temp -> retrive_flag = false;

    struct sigaction handler;
	struct sigaction oldHandler;

    stack_t stack;
	stack_t oldStack;
	
	//new stack created
	struct rlimit limit;

	getrlimit(RLIMIT_STACK, &limit);
	stack.ss_flags = 0;
	stack.ss_size = limit.rlim_cur;
	stack.ss_sp = malloc(limit.rlim_cur);	
	sigaltstack(&stack, &oldStack);	
	handler.sa_handler = &execute;
	handler.sa_flags = SA_ONSTACK;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGUSR1, &handler, &oldHandler);
	
	setter = temp;	
	raise(SIGUSR1);
	setter = NULL;	
	
	sigaltstack(&oldStack, 0);
	sigaction(SIGUSR1, &oldHandler, 0);	
	temp->stack = stack.ss_sp;

	enqueue(&ready, temp);
	thread->tid = temp->tid;
	counting_thread++;
	
	unblocking_sigalaram();
	return 0;
}

void execute(int signum) {
	assert(signum == SIGUSR1);
	if(setter == NULL) {
		exit(0);	
	}
	
	if (setjmp(setter->context)) {
		
		unblocking_sigalaram();
	
		if(setjmp(buf[current->tid])){
		}
		else{
			current->retrive_values = current->routine_start(current->arg);
			current -> retrive_flag = true;
		}
		current->active = 0;		
		blocking_sigalaram();
		enqueue(&completed, current);
		current = dequeue(&ready);
		
		longjmp(current->context, 1);
	}
	return;
}


// it is used to set time
struct itimerval timer;
static bool init_timer() {
	//Installing signal handler
	sigset_t all;
	
	sigemptyset(&all);
	sigaddset(&all, SIGALRM);

	const struct sigaction alarm = {
		.sa_handler = &scheduler,
		.sa_mask = all,
		.sa_flags = SA_RESTART
	};

	struct sigaction old;
	if(sigaction(SIGALRM, &alarm, &old) == -1);

	// it is used  10 microseconds for interval
	struct itimerval timer = {
		{ 0, 10 },	
		{ 0, 1 }	
	};
	
	if (setitimer(ITIMER_REAL, &timer, NULL) == - 1) {
		sigaction(SIGALRM, &old, NULL);
		return false;
	}
	return true;
}

// scheduler is  called at each alarm 
void scheduler() {
	if(setjmp(current->context) && sync_flag == 2) {
		sync_flag = 1;	
		return;
	}
	else {
		int flag; 		
		if(current->tid == 0)
			flag = 1;
		
		enqueue(&ready, current);		
		current = dequeue(&ready);
	
		if(flag == 1 && current->tid == 0)
			return;
		
		sync_flag = 2;	
		longjmp(current->context, sync_flag);
	}
	return;
}