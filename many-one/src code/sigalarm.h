#ifndef SIGALARM_H
#define SIGALARM_H

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

///functions for internal system of scheduling 
void unblocking_sigalaram();
void blocking_sigalaram();

#endif