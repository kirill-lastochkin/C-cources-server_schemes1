#include "service.h"

void* Working(void* arg)
{
    int r,i,chk,sock_id;
    char buf[BUFSIZE];
//    InitWorkServer();
#ifdef TCP
    int id;
    id=*((int*)arg);
#endif
#ifdef UDP
    sock_id=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in my_addr,cl_addr;
    int y=1;
    my_addr.sin_addr=((struct sockaddr_in*)arg)->sin_addr;
    my_addr.sin_family=((struct sockaddr_in*)arg)->sin_family;
    my_addr.sin_port=((struct sockaddr_in*)arg)->sin_port;
    setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
    chk=bind(sock_id,(struct sockaddr*)&my_addr,sizeof(struct sockaddr_in));
    if(chk<0)
        perror("bind srv\n");
    socklen_t len;
    len=sizeof(struct sockaddr_in);
    printf("my port is %d\n",ntohs(my_addr.sin_port));
#endif


    while(1)
    {
#ifdef TCP
        chk=recv(id,buf,BUFSIZE*sizeof(char),0);
#endif
#ifdef UDP
        chk=recvfrom(sock_id,buf,BUFSIZE*sizeof(char),0,(struct sockaddr*)&cl_addr,&len);
#endif
        if(chk==-1)
        {
            perror("receive\n");
            continue;
        }
        else
        {
            r=rand()%2;
            if(r)
            {
                for(i=0;i<BUFSIZE;i++)
                {
                    if(buf[i]>='a'&&buf[i]<='z')
                        buf[i]-=32;
                }
            }
            else
            {
                for(i=0;i<BUFSIZE;i++)
                {
                    if(buf[i]>='A'&&buf[i]<='Z')
                        buf[i]+=32;
                }
            }
#ifdef TCP
            chk=send(id,buf,BUFSIZE*sizeof(char),0);
            if(chk==-1)
            {
                perror("send\n");
                continue;
            }
#endif
#ifdef UDP
            chk=sendto(sock_id,buf,BUFSIZE*sizeof(char),0,(struct sockaddr*)&cl_addr,len);
            if(chk==-1)
            {
                perror("send\n");
                continue;
            }
#endif
            printf("send %s\n",buf);
        }
    }
}

pid_t ForkCreate(void* (*func)(void*),void *arg)
{
    pid_t pid;
    pid=fork();
    if(pid==0)
    {
        func(arg);
        exit(1);
    }
    else
    {
        return pid;
    }
}
