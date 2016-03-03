#include "service.h"

int sock_id,port;
struct sockaddr_in serv_addr;
#ifdef UDP
struct sockaddr_in newsrv; //адрес выданного слушателем сервера
#endif

//открытия сокета
void InitClient(void)
{
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
    //

}

//подключение к серверу
//ТСР - просто коннект, UDP - ждем ответ от сервера, из которого получаем
//адрес выделенного нам сервера
int ConnectServer(void)
{
    int chk,y=1;

    chk=connect(sock_id,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in));
    if(chk<0)
    {
        perror("connect\n");
        return -1;
    }
#ifdef UDP
    socklen_t len=sizeof(struct sockaddr_in);
    chk=send(sock_id,&y,sizeof(int),0);
    if(chk<0)
    {
        perror("csend\n");
        return -1;
    }
    chk=recv(sock_id,(struct sockaddr*)&newsrv,sizeof(struct sockaddr_in),0);
    if(chk<0)
    {
        perror("receive\n");
        return -1;
    }
    printf("port %d\n",ntohs(newsrv.sin_port));
    chk=connect(sock_id,(struct sockaddr*)&newsrv,len);
    if(chk<0)
    {
        perror("connect to ner srv\n");
        return -1;
    }
#endif
    return 0;
}

//рабочая функция клиента
//просто шлем серверу строчку введенную в качеству параметра
void ClientWork(char *arg)
{
    char buf[BUFSIZE];
    int cnt,chk;
    strncpy(buf,arg,BUFSIZE-1);
    for(cnt=0;;cnt++)
    {
        send(sock_id,buf,BUFSIZE*sizeof(char),0);
        chk=recv(sock_id,buf,BUFSIZE*sizeof(char),0);
        if(chk<0)
        {
            perror("rcv\n");
            continue;
        }
        printf("%s\n",buf);
        //usleep(1000);
        sleep(2);
    }
#ifdef TCP
    //close(id);
#endif
    printf("end!\n");
}

void ClientEnd(void)
{
    close(sock_id);
}
