#include "thread.h"

static int counting_thread = 0;	
bool join_and_wait[MAX_THREADS][MAX_THREADS]; 
thread_user *head; 	
static int current_pid;	

// set  join_and_wait two dimensional array and head to NULL 
void init(){
	head = NULL;
	int i, j;
	for(i = 0; i < 40; i++){
		for(j = 0; j < 40; j++){
			join_and_wait[i][j] = false;
		}
	}
}

// removing  node from queue point out by tid
thread_user* Noderemoving(int tid)
{
	if(head == NULL)
		return NULL;
	thread_user* p = head;
	if(p -> next == p){
		if(p -> tid == tid)
		{	
			head = NULL;
			return p;
		}
		return NULL;
	}
	if(p -> tid == tid){
		head = p -> next;
		p -> prev -> next = p -> next;
		p -> next -> prev = p -> prev;
		return p;
	}
	do{
		if(p -> tid == tid){
			p -> prev -> next = p -> next;
			p -> next -> prev = p -> prev; 
			return p;
		}
		p = p -> next;
	}while(p != head);
	return NULL;
}

// creating  new Thread control block
thread_user* newNode(){
	thread_user *p = (thread_user*)malloc(sizeof(thread_user));
	
	if(p == NULL){
		printf("Memory allocation failure!!\n");
		exit(0);
	}
	p -> next = p -> prev = p;
	return p;
}
int gettidfrompid(int pid){
	if(head == NULL)
		return -1;
	thread_user* p = head;
	if(p -> next == p){
		if(p -> pid == pid){
			return p -> tid;
		}
		return -1;
	}
	do{
		if(p -> pid == pid){
			return p -> tid;
		}
		p = p -> next;
		
	}while(p != head);

	return -1;	
}

// print() function used  to print queue
void print(){
	thread_user *p = head;
	int i = 0;
	do{
		p = p -> next;
		printf("%d\n",p->pid);
	}while(p != head);
}

// executing the start function
int execution_of_function(void *p) {
	thread_user *thr = (thread_user*)p;
	thr->retrive_values = thr->start(thr->args);
	return 0;
}

// adding  job to job queue for the maintianance of doubly linked list 
void add(thread_user *p){
	if(head == NULL){
		head = p;
		return;
	}	
	head -> prev -> next = p;
	p -> prev = head -> prev;
	head -> prev = p;
	p -> next = head;
}

// it is used for checking if another thread is already waiting to join with given thread
// it is also used for error checking in thread join
bool anotherThreadAlreadyWaitingToJoinWithThisThread(int waitertid){
	for(int i = 0; i < 10; i++){
		if(join_and_wait[i][waitertid]){
			return true;
		}
	}
	return false;
}

//  getting  the node, we dont have to remove it
thread_user* getNodeUsingPid(int pid)
{
	if(head == NULL)
		return NULL;
	thread_user* p = head;
	if(p -> next == p){
		if(p -> pid == pid){
			return p;
		}
		return NULL;
	}
	do{
		if(p -> pid == pid){
			return p;
		}
		p = p -> next;
		
	}while(p != head);

	return NULL;
}

// create a new user level thread and  it is give it to clone system call 
int creating_thread(thread_t *thread, void *(*routine_start) (void *), void *arg) {
	thread_user *p = newNode();
	add(p);

	p -> stack = malloc(THREAD_STACK);
	if(p -> stack == NULL){
		printf("Memory allocation failure!!\n");
		exit(0);
	}
	p -> start = routine_start;
	p -> args = arg;
	p -> retrive_values = NULL;
	p -> pid = clone(&execution_of_function, (char*) p -> stack + THREAD_STACK, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, (void*)(p));
	p -> tid = counting_thread;
	p -> retrive_flag = true;
	thread -> pid = p -> pid;
	if(p -> pid == -1) {
		free(p -> stack);
		return -1;
	}
	thread -> tid = counting_thread;
	counting_thread++;
	return 0;	
}

// check if given thread identifier is valid
bool validthreadid(int tid){
	if(head == NULL)
		return -1;
	thread_user* p = head;
	if(p -> next == p){
		if(p -> tid == tid){
			return true;
		}
		return false;
	}
	do{
		if(p -> tid == tid){
			return true;
		}
		p = p -> next;
		
	}while(p != head);

	return false;	
}

// discontinue the given thread and storing  the return value in thread control block
void exit_from_thread(void *retrive_values){
	current_pid = getpid();
	thread_user *p = getNodeUsingPid(current_pid);
	if(p == NULL)
		return;	
	
	p -> retrive_flag = false;
	p -> retrive_values = retrive_values;
	// killing the  associated process
	kill(current_pid, 9);
	p -> stack = NULL;
}

// wait till the associated process with given thread gets completed
int joining_thread(thread_t thread, void **np) {

	pid_t waitertid = -1;
	int current_pid = getpid();
	waitertid = gettidfrompid(current_pid);

	if(!validthreadid(thread.tid)){
		return ESRCH;
	}		
	if(anotherThreadAlreadyWaitingToJoinWithThisThread(waitertid)){
		return EINVAL;
	}
	//  checking deadlock
	if(waitertid != -1){
		
		if(join_and_wait[thread.tid][waitertid] == true){
			return EDEADLK;
		}
		join_and_wait[waitertid][thread.tid] = true;
	}
	
	int curr = thread.tid;
	int i;
	int status;
	// it will wait till the process gets completed
	waitpid(thread.pid, &status, 0);
	if(&status == NULL)
		return 0;
	// extracting  the job from job queue
	thread_user *p = Noderemoving(thread.tid);
	
	if(np != NULL)
	{
		if(p != NULL){
			
			if(p -> retrive_flag == true)
			{
				*np = &(p -> retrive_values);
			}
			
			else{
				*np = (p -> retrive_values);
			}
		}
	}
	join_and_wait[waitertid][thread.tid] = false;
	// free the contents
	free(p);
	return 0;
}
// sending signal to specific thread
int killing_thread(thread_t thread, int signo){
	return kill(thread.pid, signo);
}

