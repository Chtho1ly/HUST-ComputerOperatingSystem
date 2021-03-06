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

#define ADDR "."
#define SOURCE_FILE "./source.pptx"
#define DEST_FILE "./destination.pptx"
#define BUF_LEN 512
#define LIST_LEN 8

using namespace std;

struct node
{
	node *next;
	char *data;
	int len;
	bool last;
};

int idWrite, idRead;
key_t keyPtr, keyData;

int write_Process()
{
	//绑定共享内存
	//声明数据部分内存
	int idData;
	char *headData;
	idData = shmget(keyData, sizeof(char) * BUF_LEN * LIST_LEN, 0666 | IPC_CREAT);
	if (idData == -1)
	{
		fprintf(stderr, "shmat read Data failed\n");
		exit(EXIT_FAILURE);
	}
	headData = (char *)shmat(idData, 0, 0);
	//声明指针部分内存
	int idPtr;
	node *head;
	idPtr = shmget(keyPtr, sizeof(node) * LIST_LEN, 0666 | IPC_CREAT);
	if (idPtr == -1)
	{
		fprintf(stderr, "shmat write failed\n");
		exit(EXIT_FAILURE);
	}
	head = (node *)shmat(idPtr, 0, 0);
	//写功能实现ifstream streamWrite;
	ifstream streamWrite;
	streamWrite.open(SOURCE_FILE,ios_base::binary);
	if (!streamWrite.is_open())
	{
		printf("Fail in opening source.txt\n");
		exit(1);
	}
	while (true)
	{
		P(idWrite, 0);
		streamWrite.read(head->data, BUF_LEN - 1);
		head->len = streamWrite.gcount();
		head->data[head->len] = 0;
		head->last = false;
		//cout << head->len << ": " << head->data << endl;
		//cout << head->data;
		if (streamWrite.eof())
		{
			head->last = true;
			streamWrite.close();
			V(idRead, 0);
			break;
		}
		V(idRead, 0);
		head = head->next;
	}
	printf("Read finished.\n");
}

int read_Process()
{
	//绑定共享内存
	//声明数据部分内存
	int idData;
	char *headData;
	idData = shmget(keyData, sizeof(char) * BUF_LEN *  LIST_LEN, 0666 | IPC_CREAT);
	if (idData == -1)
	{
		fprintf(stderr, "shmat read Data failed\n");
		exit(EXIT_FAILURE);
	}
	headData = (char *)shmat(idData, 0, 0);
	//声明指针部分内存
	int idPtr;
	node *head;
	idPtr = shmget(keyPtr, sizeof(char) *LIST_LEN, 0666 | IPC_CREAT);
	if (idPtr == -1)
	{
		fprintf(stderr, "shmat read Data failed\n");
		exit(EXIT_FAILURE);
	}
	head = (node *)shmat(idPtr, 0, 0);
	//读功能实现
	ofstream streamRead;
	streamRead.open(DEST_FILE,ios_base::binary);
	if (!streamRead.is_open())
	{
		printf("Fail in opening destination.txt\n");
		exit(1);
	}
	while (true)
	{
		P(idRead, 0);
		streamRead.write(head->data, head->len);
		V(idWrite, 0);
		if (head->last)
		{
			streamRead.close();
			break;
		}
		head = head->next;
	}
	printf("Write finished.\n");
}

int main()
{
	//绑定共享内存
	//声明数据部分内存
	int idData;
	char *headData;
	keyData = ftok(ADDR, 0x10);
	idData = shmget(keyData, sizeof(char)  * BUF_LEN* LIST_LEN, 0666 | IPC_CREAT);
	if (idData == -1)
	{
		fprintf(stderr, "shmat main Data failed\n");
		exit(EXIT_FAILURE);
	}
	headData = (char *)shmat(idData, 0, 0);
	//声明指针部分内存
	int idPtr;
	node *head;
	keyPtr = ftok(ADDR, 0x11);
	idPtr = shmget(keyPtr, sizeof(node) * LIST_LEN, 0666 | IPC_CREAT);
	cout << idPtr << endl;
	if (idPtr == -1)
	{
		fprintf(stderr, "shmat main Data failed\n");
		exit(EXIT_FAILURE);
	}
	head = (node *)shmat(idPtr, 0, 0);
	//初始化链表指针结构
	node *cur = head;
	char *curData = headData;
	for (int i = 0; i < LIST_LEN - 1; cur += sizeof(node), curData += sizeof(char) * BUF_LEN, i++)
	{
		cur->next = cur + sizeof(node);
		cur->data = curData;
	}
	cur->next = head;
	cur->data = curData;
	//写信号灯，初值为0
	key_t keyWrite = ftok(ADDR, 0x01);
	idWrite = create_Sem(keyWrite, 1);
	set_N(idWrite, 0, LIST_LEN);
	//读信号灯，初值为0
	key_t keyRead = ftok(ADDR, 0x02);
	idRead = create_Sem(keyRead, 1);
	set_N(idRead, 0, 0);
	//创建线程
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
	if (shmctl(idPtr, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(remive) main failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}