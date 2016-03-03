#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <wait.h>
#include <unistd.h>

#define MAXNUMOFCLIENTS 100
#define BUFSIZE 50

//������� �������� �� 4� ��������� fork/threads � tcp/udp
#define FORK
//#define THR
//#define TCP
#define UDP

//------------���������-------------
void InitListener(void);
int WaitForClient(void); //���� ������� �� �������������, ������� ����������, �������������� � ����
void SigIntHandler(); //�������� ����������
void ExitFunc(); //������� ����������� � exit
void SigPipeHandler(); //�������� ����������

//------------������-�����-------------
void* Working(void* arg);// ����������� ������� �������-�����������
pid_t ForkCreate(void* (*func)(void*),void *arg); //������ �������� ������

//------------������-------------------
void InitClient(void); //��������� �������
int ConnectServer(void); //�������� ������� � ��������� ������ �� �������
void ClientWork(char *arg); //����� ����������� � ��������
void ClientEnd(void); //����� ������
