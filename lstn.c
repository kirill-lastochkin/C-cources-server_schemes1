#include "service.h"

int sock_id,port,id,clcnt;
struct sockaddr_in serv_addr,cl_addr;
#ifdef UDP
struct sockaddr_in serv_addr1; //дополнительный адрес, который отдаем клиенту
#endif
#ifdef THR
pthread_t *thr[MAXNUMOFCLIENTS];  //запасаем указатели для потоков
#endif
#ifdef FORK
pid_t pids[MAXNUMOFCLIENTS];
#endif

//инициализация слшателя
void InitListener(void)
{
    int y=1,chk;
    clcnt=0;
#ifdef FORK
    for(chk=0;chk<MAXNUMOFCLIENTS;chk++)
    {
        pids[chk]=0;
    }
#endif
    atexit(ExitFunc);
    //создаем и биндим сокет
#ifdef TCP
    sock_id=socket(AF_INET,SOCK_STREAM,0);
#endif
#ifdef UDP
    sock_id=socket(AF_INET,SOCK_DGRAM,0);
#endif
    if(sock_id<0)
            perror("socket1\n");
    port=5000;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(port);
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    chk=bind(sock_id,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in));
    if(chk<0)
        perror("bind\n");
#ifdef TCP
    chk=listen(sock_id,MAXNUMOFCLIENTS);
    if(chk<0)
        perror("listen\n");
#endif
    //подменяем сигналы
    signal(SIGINT,SigIntHandler);
    signal(SIGPIPE,SigPipeHandler);
}

//основная ыункция - ждать запрос от клиента
//ждем сигнала от клиента, потом
//ТСР - принимаем его, перекидываем выделенный идентификатор созданному потоку/процессу
//UDP - назначаем новый адрес с портом, отправляем его в ответе клиенту, создаем поток/процесс
int WaitForClient(void)
{
    socklen_t len;
    int buf,chk;
    len=sizeof(struct sockaddr_in);
    while(1)
    {
#ifdef TCP
        id=accept(sock_id,(struct sockaddr*)&cl_addr,&len);
        if(id<0)
        {
            perror("accept\n");
        }
        else
        {
            printf("client accepted\n");
#ifdef THR
            thr[clcnt]=(pthread_t*)calloc(1,sizeof(pthread_t));
            if(thr[clcnt]==NULL)
            {
                perror("calloc\n");
            }
            pthread_create(thr[clcnt],NULL,Working,&id);
#endif
#ifdef FORK
            pids[clcnt]=ForkCreate(Working,&id);
            printf("with pid %ld\n",(long)pids[clcnt]);
#endif

        }
#endif
#ifdef UDP
        chk=recvfrom(sock_id,&buf,sizeof(int),0,(struct sockaddr*)&cl_addr,&len);
        if(chk==-1)
        {
            perror("receive connect\n");
            return -1;
        }
        printf("client accepted\n");
        serv_addr1.sin_addr=serv_addr.sin_addr;
        serv_addr1.sin_family=serv_addr.sin_family;
        serv_addr1.sin_port=htons(ntohs(serv_addr.sin_port)+clcnt+1);
        printf("port given %d\n",ntohs(serv_addr1.sin_port));
        chk=sendto(sock_id,&serv_addr1,sizeof(struct sockaddr_in),0,(struct sockaddr*)&cl_addr,len);
        if(chk==-1)
        {
            perror("send connect\n");
            return -1;
        }
#ifdef THR
        thr[clcnt]=(pthread_t*)calloc(1,sizeof(pthread_t));
        if(thr[clcnt]==NULL)
        {
            perror("calloc\n");
        }
        chk=pthread_create(thr[clcnt],NULL,Working,&serv_addr1);
        if(chk==-1)
        {
            perror("thread create\n");
        }
#endif
#ifdef FORK
        pids[clcnt]=ForkCreate(Working,&serv_addr1);
#endif

#endif
        clcnt++;
    }
}

//просто выход
void SigIntHandler()
{
    exit(1);
}

//обработка ситуации, когда все клиенты отсоединяются
void SigPipeHandler()
{

    int i;
    for(i=0;i<clcnt;i++)
    {
#ifdef THR
        if(thr[i]!=NULL)
        {
            pthread_cancel(*thr[i]);
            free(thr[i]);
        }
#endif
#ifdef FORK
        kill(pids[i],SIGKILL);
        pids[i]=0;
#endif
    }
    clcnt=0;
}

//фнкция выхода
void ExitFunc()
{
#ifdef THR
    int i;
    for(i=0;i<clcnt;i++)
    {
        if(thr[i]!=NULL)
        {
            pthread_cancel(*thr[i]);
            free(thr[i]);
        }
    }
#endif
    close(sock_id);
#ifdef FORK
    killpg(getpid(),SIGINT);
#endif
}
