#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define N 4

pid_t Pid1, Pid2;
int count = 0;
int fd[2];
void son_Sig(int sig_no)
{
    if (sig_no == SIGUSR1)
    {
        printf("Process 1 recv: %d\n", count);
        printf("Child Process 1 is Killed by Parent!\n");
    }
    else if (sig_no == SIGUSR2)
    {
        printf("Process 2 recv: %d\n", count);
        printf("Child Process 2 is Killed by Parent!\n");
    }
    close(fd[0]);
    close(fd[1]);
    exit(0);
}

void par_Sig()
{
    int status;
    kill(Pid1, SIGUSR1);
    kill(Pid2, SIGUSR2);
    waitpid(Pid1,&status,0);
    waitpid(Pid2,&status,0);
    close(fd[0]);
    close(fd[1]);
    exit(0);
}

int main()
{
    int recv = 0;
    pipe(fd);
    Pid1 = fork();
    if (Pid1 == 0) //son1
    {
        signal(SIGUSR1, son_Sig);
        signal(SIGINT, SIG_IGN);
        while (1)
        {
            read(fd[0], &recv, sizeof(recv));
            printf("Process 1: %d\n", recv);
            count++;
        }
    }
    else
    {
        Pid2 = fork();
        if (Pid2 == 0) //son2
        {
            signal(SIGUSR2, son_Sig);
            signal(SIGINT, SIG_IGN);
            while (1)
            {
                read(fd[0], &recv, sizeof(recv));
                printf("Process 2: %d\n", recv);
                count++;
            }
        }
        else //father
        {
            signal(SIGINT, par_Sig);
            for (int i = 1; i <= N; i++)
            {
                write(fd[1], &i, sizeof(i));
                sleep(1);
            }
            printf("Job Done\n");
            kill(getpid(), SIGINT);
        }
    }
}