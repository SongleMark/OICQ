#ifndef CHART_H
#define CHART_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdarg.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>

class Chart
{
public:
    explicit Chart(char *ip, unsigned short port);
    virtual ~Chart();
    bool Init();

private:
    char *mIp;
    int mSockaddrfd;
    unsigned short mPort;
    sockaddr_in mSaddr;
    sockaddr_in mCaddr;
};

#endif