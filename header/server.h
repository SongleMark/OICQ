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

//打包发送结构体
struct Package
{
	int type;
	int len;
	char data[0];
};

//已连接客户端结构体
typedef struct ClientNode
{
    int fd;
    sockaddr_in caddr;
    ClientNode *next;
}ClientNode;

//在线用户结构体
struct OnlineUser  
{
    int id;
    char name[32];
    char ip[32];
    unsigned short port;
    OnlineUser *next;
};

//消息结构体
struct Message
{
    int recvid;//接受者的id(文件描述符)
    int sendid;//发送者的id(文件描述符)
    char *name;//发送者的名字
    char message[100];//发送的信息
};

class Server
{
public:
    explicit Server(unsigned short port = 8000, bool stop = false, ClientNode *head = NULL, OnlineUser *uhead = NULL);
    virtual ~Server();
    bool Init();
    void DeInit();
    void SetPort();
    bool Connect();
    void DisConnect();
    bool SelectClient();
    bool WriteData(const int data);
    bool WriteData();
    bool DealMessage();
    static void RecvSignal(int sig);
    static void *Thread(void *task);
    void GetStorageFromManager(StorageService *storage);
    void CreateUserNode(int id, char *name);
    void RemoveUnlineUserNode(int id);

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
    OnlineUser *mUserhead;
    enum
    {
		ZERO,
		REGISTER,
		LOGIN,
        GETONLINEUSER,
        MESSAGE
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