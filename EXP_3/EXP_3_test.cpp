#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "sem.h"

#define ADDR "./EXP_3"
#define MAX 20

using namespace std;

struct intst
{
	int n;
};

int idEven, idOdd;
key_t keyMem;

int even_Print()
{
	int idI;
	int *i;
	idI = shmget(keyMem, sizeof(int), 0666 | IPC_CREAT);
	if (idI == -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	i = (int *)shmat(idI, 0, 0);
	for (; *i < MAX;)
	{
		P(idEven, 0);
		if (*i < MAX)
		{
			printf("Even %d\n", *i);
			(*i) += 1;
		}
		V(idOdd, 0);
	}
}

int odd_Print()
{
	int idI;
	int *i;
	idI = shmget(keyMem, sizeof(int), 0666 | IPC_CREAT);
	if (idI == -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	i = (int *)shmat(idI, 0, 0);
	for (; *i < MAX;)
	{
		P(idOdd, 0);
		if (*i < MAX)
		{
			printf("Odd %d\n", *i);
			(*i) += 1;
		}
		V(idEven, 0);
	}
}

int main()
{
	int idI;
	int *i;
	keyMem = ftok(ADDR, 0x10);
	idI = shmget(keyMem, sizeof(int), 0666 | IPC_CREAT);
	if (idI == -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	i = (int *)shmat(idI, 0, 0);
	*i = 0;
	//打印偶数信号灯，初值为1
	key_t keyEvenId = ftok(ADDR, 0x01);
	idEven = create_Sem(keyEvenId, 1);
	set_N(idEven, 0, 1);
	//打印奇数信号灯，初值为0
	key_t keyOddId = ftok(ADDR, 0x02);
	idOdd = create_Sem(keyOddId, 1);
	set_N(idOdd, 0, 0);
	pid_t pid;
	if (fork() == 0)
	{
		//偶数子进程
		even_Print();
		return 0;
	}
	else if (fork() == 0)
	{
		//奇数子进程
		odd_Print();
		return 0;
	}
	int *t;
	wait(t);
	wait(t);
	return 0;
}