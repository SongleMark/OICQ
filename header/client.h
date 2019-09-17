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

struct Package
{
	int type;
	int len;
	char data[0];
};

struct RegisteInfo
{
    int id;
    int64_t telphone;
    char name[32];
	char password[32];
};

struct LoginInfo
{
	char name[66];
	char password[66];
};

class Client
{
	public:
		explicit Client(char *ip = (char *)"127.0.0.1", unsigned short port = 8000);
		virtual ~Client();
		bool Init();
		void DeInit();
		bool Connect();
		void DisConnect();
		bool WriteData(char *data);
		bool WriteData(RegisteInfo registinfo);
		bool WriteData(LoginInfo logininfo);
		bool ReadData(const char *name = (const char *)"");
		static void RecvSignal(int sig);

	private:
		enum{
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
		int mSocketfd;
		char *mIp;
		unsigned short mPort;
		sockaddr_in mSaddr;
		bool mIsStop;
		RegisteInfo mRegister;

};

#endif