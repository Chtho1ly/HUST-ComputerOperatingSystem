#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "sem.h"

#define ADDR "./EXP_3"
#define SOURCE_FILE "./source.txt"
#define BUF_LEN 10
#define LIST_LEN 4

using namespace std;

struct node
{
	node *next;
	//char data[BUF_LEN];
	int data;
};

int idWrite, idRead;
key_t keyMem;

int write_Process()
{
	//绑定共享内存
	int idI;
	node *head;
	idI = shmget(keyMem, sizeof(node) * LIST_LEN, 0666 | IPC_CREAT);
	if (idI == -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	head = (node *)shmat(idI, 0, 0);
	//写功能实现
	int sourceData[6] = {12, 453, 0, 9878, 12345, -1};
	for (int i = 0; i < 6; i++, head = head->next)
	{
		P(idWrite, 0);
		head->data = sourceData[i];
		V(idRead, 0);
	}
}

int read_Process()
{
	//绑定共享内存
	int idI;
	node *head;
	idI = shmget(keyMem, sizeof(node) * LIST_LEN, 0666 | IPC_CREAT);
	if (idI == -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	head = (node *)shmat(idI, 0, 0);
	//读功能实现
	while (true)
	{
		int data;
		P(idRead, 0);
		data = head->data;
		V(idWrite, 0);
		if (data == -1)
			break;
		cout << data << endl;
		head = head->next;
	}
}

int main()
{
	int idI;
	node *head;
	keyMem = ftok(ADDR, 0x10);
	idI = shmget(keyMem, sizeof(node) * LIST_LEN, 0666 | IPC_CREAT);
	if (idI == -1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	head = (node *)shmat(idI, 0, 0);
	//初始化链表指针结构
	node *cur = head;
	for (int i = 0; i < LIST_LEN - 1; cur += sizeof(node), i++)
		cur->next = cur + sizeof(node);
	cur->next = head;
	//测试代码
	/*
	while(true)
	{
		cout<<cur<<endl;
		cur = cur->next;
		sleep(1);
	}
	*/

	//写信号灯，初值为LIST_LEN
	key_t keyWrite = ftok(ADDR, 0x01);
	idWrite = create_Sem(keyWrite, 1);
	set_N(idWrite, 0, LIST_LEN);
	//读信号灯，初值为0
	key_t keyRead = ftok(ADDR, 0x02);
	idRead = create_Sem(keyRead, 1);
	set_N(idRead, 0, 0);
	pid_t pid;
	if (fork() == 0)
	{
		//写子进程
		write_Process();
		return 0;
	}
	else if (fork() == 0)
	{
		//读子进程
		read_Process();
		return 0;
	}
	int *t;
	wait(t);
	wait(t);

	return 0;
}