#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include "sem.h"
#define ADDR "./EXP_2"
#define WINDOW_NUM 5
#define TICKET_NUM 1000

using namespace std;

int i = 0;
int idSelling;
int idInit;
int count[WINDOW_NUM];
int sum = 0;

void *window(void *arg)
{
	int num = *(int *)arg;
	V(idInit, 0);
	while (sum < TICKET_NUM)
	{
		P(idSelling, 0);
		if (sum < TICKET_NUM)
		{
			count[num]++;
			sum++;
			printf("Window %d sells %d.\n", num, sum);
		}
		V(idSelling, 0);
	}
	printf("Window %d sells %d in total.\n", num, count[num]);
}

int main(void)
{
	//可以售卖信号灯，初值为1
	key_t keySelling = ftok(ADDR, 0x1);
	idSelling = create_Sem(keySelling, 1);
	set_N(idSelling, 0, 1);
	//初始化窗口信号灯，初值为1
	key_t keyInit = ftok(ADDR, 0x2);
	idInit = create_Sem(keyInit, 1);
	set_N(idInit, 0, 0);
	//声明线程号
	pthread_t pidWindow[WINDOW_NUM];
	int ret = 0;
	//创建售票的线程
	for (int numInit = 0; numInit < WINDOW_NUM;)
	{
		ret = pthread_create(&pidWindow[numInit], NULL, window, &numInit);
		if (ret != 0)
		{
			printf("Create pthread error!\n");
			exit(1);
		}
		P(idInit, 0);
		numInit++;
	}
	//等待线程结束
	for (int i = 0; i < WINDOW_NUM; i++)
		pthread_join(pidWindow[i], NULL);
	//删除信号灯
	destroy_Sem(idInit);
	destroy_Sem(idSelling);
	return 0;
}