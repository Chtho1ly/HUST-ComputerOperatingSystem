#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include "sem.h"
#define keyEven 1
#define keyOdd 2

using namespace std;

int i = 0;
int idEven, idOdd;

void *even_Print(void *)
{
	for (; i < 100; i++)
	{
		P(idEven, 0);
		printf("Even %d.\n", i);
		V(idOdd, 0);
	}
}

void *odd_Print(void *)
{
	for (; i < 100; i++)
	{
		P(idOdd, 0);
		printf("Odd %d.\n", i);
		V(idEven, 0);
	}
}

int main(void)

{
	//打印偶数信号灯，初值为1
	idEven = create_Sem(keyEven, 1);
	set_N(idEven, 0, 1);
	//打印奇数信号灯，初值为0
	idOdd = create_Sem(keyOdd, 1);
	set_N(idOdd, 0, 0);
	pthread_t pidEven,
		pidOdd;
	int ret = 0;
	//创建打印偶数的线程
	ret = pthread_create(&pidEven, NULL, even_Print, NULL);
	if (ret != 0)
	{
		printf("Create pthread Even error!\n");
		exit(1);
	}
	//创建打印奇数的线程
	ret = pthread_create(&pidOdd, NULL, odd_Print, NULL);
	if (ret != 0)
	{
		printf("Create pthread Odd error!\n");
		exit(1);
	}
	//等待线程结束
	pthread_join(pidEven, NULL);
	pthread_join(pidOdd, NULL);
	//删除信号灯
	destroy_Sem(idEven);
	destroy_Sem(idOdd);
	return 0;
}