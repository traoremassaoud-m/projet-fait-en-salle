#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
int stt;

pid_t creation_processus(void)
{

    pid_t pid;

    do {
        pid = fork();
    } while ((pid == -1) && (errno == EAGAIN));

    return pid;
}


void processus_enfant()
{
    int i=0;

    for(i=0;i<3;i++){
      printf(" Le PID du fils est %d.\n",(int) getpid());
    }
}

void processus_pere(int pid_enfant)
{
    printf(" Le PID du père est %d.\n",(int) getpid());

    if (wait(&stt) == -1) {
	    perror("wait :");
	    exit(EXIT_FAILURE);
    }
}

int main()
{
    pid_t pid = creation_processus();

    switch (pid) {
        case -1:
            perror("fork");
            return EXIT_FAILURE;
        break;

        case 0:
            processus_enfant();
        break;

        default:
            processus_pere(pid);
	break;
    }

    return 0;
}
