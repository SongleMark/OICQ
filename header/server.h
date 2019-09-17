#ifndef SERVER_H
#define SERVER_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include "storage.h"

#define SIZE 1024

struct Package
{
	int type;
	int len;
	char data[0];
};

typedef struct ClientNode
{
    int fd;
    sockaddr_in caddr;
    ClientNode *next;
}ClientNode;

class Server
{
public:
    explicit Server(unsigned short port = 8000, bool stop = false, ClientNode *head = NULL);
    virtual ~Server();
    bool Init();
    void DeInit();
    bool Connect();
    void DisConnect();
    bool SelectClient();
    bool WriteData(const int data);
    bool ReadData();
    static void RecvSignal(int sig);
    static void *Thread(void *task);
    void GetStorageFromManager(StorageService *storage);

private:
    int mSocketfd;
    int mMaxfd;
    int mAcceptfd;
    int mTempfd;
    int mAddrlen;
    bool mIsStop;
    sockaddr_in mSaddr;
    sockaddr_in mCaddr;
    fd_set mSet;
    fd_set mReadset;
    unsigned short mPort;
    ClientNode *mHead;
    ClientNode *mNode;
    pthread_t mPthread;
    char mData[SIZE];
    RegisteInfo mRegistinfo;
    LoginInfo mLogininfo;
    StorageService *mStorage;
    enum
    {
		ZERO,
		REGISTER,
		LOGIN
	};
    enum
    {
        RFAILED,
        RSUCCESSED,
        LFAILED,
        LSUCCESSED
    };

};

#endif