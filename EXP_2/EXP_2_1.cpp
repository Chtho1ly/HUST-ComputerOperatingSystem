#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include "sem.h"
#define keyEven 1
#define keyOdd 2
#define keyAdd 3

using namespace std;

int i = 0;
int sum;
int idEven, idOdd, idAdd;

void *even_Print(void *)
{
	while (true)
	{
		P(idEven, 0);
		if (i <= 100)
		{
			printf("Even %d.\n", sum);
		}
		else
		{
			V(idAdd, 0);
			return NULL;	
		}
		V(idAdd, 0);
	}
}

void *odd_Print(void *)
{
	while (true)
	{
		P(idOdd, 0);
		if (i <= 100)
		{
			printf("Odd %d.\n", sum);
		}
		else
		{
			V(idAdd, 0);
			return NULL;
		}
		V(idAdd, 0);
	}
}

int main(void)

{
	//打印偶数信号灯，初值为0
	idEven = create_Sem(keyEven, 1);
	set_N(idEven, 0, 0);
	//打印奇数信号灯，初值为0
	idOdd = create_Sem(keyOdd, 1);
	set_N(idOdd, 0, 0);
	//累加信号灯，初值为1
	idAdd = create_Sem(keyAdd, 1);
	set_N(idAdd, 0, 1);
	pthread_t pidEven, pidOdd;
	pid_t pid1 = 0;
	pid_t pid2 = 0;
	//创建打印偶数的线程
	pid1 = pthread_create(&pidEven, NULL, even_Print, NULL);
	if (pid1 != 0)
	{
		printf("Create pthread Even error!\n");
		exit(1);
	}
	//创建打印奇数的线程
	pid2 = pthread_create(&pidOdd, NULL, odd_Print, NULL);
	if (pid2 != 0)
	{
		printf("Create pthread Odd error!\n");
		exit(1);
	}
	for (i = 0, sum = 0; i < 100;)
	{
		P(idAdd, 0);
		i++;
		sum += i;
		if (sum % 2 == 0)
			V(idEven, 0);
		else
			V(idOdd, 0);
	}
	P(idAdd, 0);
	i = 200;
	V(idOdd, 0);
	V(idEven, 0);
	//等待线程结束
	pthread_join(pidEven, NULL);
	pthread_join(pidOdd, NULL);
	//删除信号灯
	destroy_Sem(idEven);
	destroy_Sem(idOdd);
	return 0;
}