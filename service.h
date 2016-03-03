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

//следует выбирать из 4х сочетаниц fork/threads и tcp/udp
#define FORK
//#define THR
//#define TCP
#define UDP

//------------слушающий-------------
void InitListener(void);
int WaitForClient(void); //ждем запроса на подсоединение, создаем обработчик, перенаправляем в него
void SigIntHandler(); //обраотка прерывания
void ExitFunc(); //функция привязанная к exit
void SigPipeHandler(); //обраотка прерывания

//------------сервер-поток-------------
void* Working(void* arg);// циклическая функция сервера-обработчика
pid_t ForkCreate(void* (*func)(void*),void *arg); //аналог создания потока

//------------клиент-------------------
void InitClient(void); //настройка сокетов
int ConnectServer(void); //отправка запроса и ождидание ответа от сервера
void ClientWork(char *arg); //обмен информацией с сервером
void ClientEnd(void); //конец работы
