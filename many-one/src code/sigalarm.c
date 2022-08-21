#include "sigalarm.h"

void unblocking_sigalaram() {
	sigset_t sigalrm;
	sigemptyset(&sigalrm);
	sigaddset(&sigalrm, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &sigalrm, NULL);}

void blocking_sigalaram() {
	sigset_t sigalrm;
	sigemptyset(&sigalrm);
	sigaddset(&sigalrm, SIGALRM);
	sigprocmask(SIG_BLOCK, &sigalrm, NULL);
}
