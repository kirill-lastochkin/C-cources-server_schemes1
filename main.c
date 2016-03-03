#include "service.h"

//для компиляции клиента: -D CLN main.c cln.c -lpthread
//для компиляции сервера: -D SRV main.c lstn.c srv.c -lpthread

int main(int argc,char* argv[])
{
#ifdef CLN
    InitClient();
    ConnectServer();
    if(argc<2)
    {
        printf("not enough parameters");
        return 0;
    }
    ClientWork(argv[1]);
    ClientEnd();
#endif
#ifdef SRV
    InitListener();
    WaitForClient();
#endif
    return 0;
}

