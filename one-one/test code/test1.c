#include "thread.h"
#include <stdio.h>
#include <unistd.h>
void* fun(void *args){
	while(1){
		printf("Hello World!\n");
		int r = 1000;
		exit_from_thread(&r);
	}
}	
void* fun11(void *args){
	printf("Hi\n");
	int r = 999;
	return r;
}
int main()
{
	init();
	thread_t t1, t2;
	creating_thread( &t1, fun , NULL);
	creating_thread( &t2, fun11 , NULL);	
	void* n;
	print();
	joining_thread(t1, &n);
	printf("\nretval from join of thread1 %d\n", *(int*)(n));
	killing_thread(t2, SIGINT);
	joining_thread(t2, &n);
	printf("\nretval from join of thread2 %d\n", *(int*)(n));
	return 0;
}
