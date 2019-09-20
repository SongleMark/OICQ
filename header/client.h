#ifndef CLIENT_H
#define CLIENT_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <algorithm>
#include <iostream>

//打包发送结构体
struct Package
{
	int type;
	int len;
	char data[0];
};

//注册信息
struct RegisteInfo
{
    int id;
    int64_t telphone;
    char name[32];
	char password[32];
};

//登录信息
struct LoginInfo
{
	char name[66];
	char password[66];
};

//在线用户链表结构体
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
    char sendname[32];//发送者的名字
    char recvname[32];//接受者的名字
    char message[100];//发送的信息
    char time[32]; //消息发送的时间
};

class Client
{
public:
	explicit Client(char *ip = (char *)"127.0.0.1", unsigned short port = 8000);
	virtual ~Client();
	bool Init();
	void DeInit();
	void SetIPandPort();
	bool Connect();
	void DisConnect();
	bool WriteData(char *data);
	bool WriteData(RegisteInfo registinfo);
	bool WriteData(LoginInfo logininfo);
	bool WriteData();
	bool WriteData(Message message);
	bool ReadData(const char *name = (const char *)"");
	bool ReadMessage();
	OnlineUser *ReadOnlineUserNode();
	static void RecvSignal(int sig);
	char *GetAddress();
	unsigned short GetPort();

private:
	enum{
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
	int mSocketfd;
	char *mIp;
	unsigned short mPort;
	sockaddr_in mSaddr;
	bool mIsStop;
	RegisteInfo mRegister;

};

#endif