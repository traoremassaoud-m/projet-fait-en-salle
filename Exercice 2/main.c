#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>


union semun {
   int  val;
   struct semid_ds *buf;
   unsigned short  *array;
   struct seminfo  *__buf;


int main(int argc, char *argv[])
{
    pid_t pid1, pid2, pid3;
    int status;
    union semun sem_arg;
    char *argp1[] = {"w", NULL, NULL};
    key_t sem_key = ftok("semfile",75);
    int semid = semget(sem_key, 1, 0666|IPC_CREAT);

    sem_arg.val = 1;
    if(semctl(semid, 0, SETVAL, sem_arg)==-1){
       perror("semctl");
       exit(1);
    }

    key_t key = ftok("shmfile",65);

    int a = 0;
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    char *str = (char*) shmat(shmid,(void*)0,0);
    sprintf(str, "%d", a);

    pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1 == 0){
        execv("./w", argp1);
    }

    else{
        char *argp2[] = {"Q1", NULL, NULL};
        pid2 = fork();
        sleep(7);
        if(pid2 < 0){
          perror("Erreur de création du second processus\n");
          pid1 = wait(&status);
          exit(EXIT_FAILURE);
        }

        if(pid2 == 0){
            execv("./w", argp2);
        }else{
            char *argp3[] = {"Q1", NULL, NULL};
            pid3 = fork();
            sleep(7);
            if(pid3 < 0){
                perror("Erreur de création du second processus\n");
                pid2 = wait(&status);
                exit(EXIT_FAILURE);
            }

            if(pid3 == 0){
                execv("./w", argp3);
            }else{
                pid1 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid1);
                pid2 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid2);
                pid3 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid3);

                a = atoi(str);
                printf("Valeur Finale de a = %d\n", a);
                shmdt(str);
                shmctl(shmid,IPC_RMID,NULL);

                if(semctl(semid, 0, IPC_RMID, NULL) == -1){
                   perror("semctl");
                   exit(1);
                }
            }
		}
    }
}
