#include "../header/client.h"
#include "../header/logger.h"

#define TAG "Client"

Client::Client(char *ip, unsigned short port):
    mIp(ip),
    mPort(port)
{
}

Client::~Client()
{
}

//客户端初始化socket->connect函数
bool Client::Init()
{
    int result = 0;
    mIsStop = false;
    signal(SIGPIPE, RecvSignal);
    LOG(TAG, "socket ...");
    mSocketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(mSocketfd < 0)
    {
        LOGE(TAG, "socket failed ...");
        return false;
    }

    mSaddr.sin_family = AF_INET;
    mSaddr.sin_port = htons(mPort);
    mSaddr.sin_addr.s_addr = inet_addr(mIp);
    LOG(TAG, "connect ...");
    result = connect(mSocketfd, (sockaddr *)&mSaddr, (socklen_t)sizeof(mSaddr));
    if(result < 0)
    {
        LOGE(TAG, "connect failed ...");
        return false;
    }

    return true;
}

void Client::DeInit()
{
    mIsStop = true;
}

bool Client::Connect()
{
    bool result = true;
    result = Init();
    if(!result)
    {
        LOGE(TAG, "init socket error can't connect ...");
        return false;
    }
    else
    {
        return true;
    }
    
    return true;
}

void Client::DisConnect()
{
    close(mSocketfd);
}

bool Client::WriteData(char *data)
{
    int result = 0;
    result = write(mSocketfd, data, strlen(data) + 1);
    if(result < 0)
    {
        LOGE(TAG, "write error");
        return false;
    }

    return true;
}

//将用户注册信息发送给服务器
bool Client::WriteData(RegisteInfo registinfo)
{
    int result = 0;
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(RegisteInfo));
    if(!pack)
    {
        LOGE(TAG, "malloc failed ...");
        return false;
    }
    pack->type = REGISTER;
    pack->len = sizeof(RegisteInfo);
    memcpy(pack->data, &registinfo, sizeof(RegisteInfo));

    result = write(mSocketfd, pack, sizeof(Package) + pack->len);
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }
    else
    {
        LOG(TAG, "write registerinfo success ...");
    }

    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        return false;
    }
    free(pack);

    return true;
}

//将登录信息发送给服务器
bool Client::WriteData(LoginInfo logininfo)
{
    int result = 0;
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(LoginInfo));
    if(NULL == pack)
    {
        LOGE(TAG, "malloc error");
        return false;
    }
    pack->type = LOGIN;
    pack->len  = sizeof(LoginInfo);
    memcpy(pack->data, &logininfo, pack->len);
    result = write(mSocketfd, pack, sizeof(Package) + pack->len);
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }
    LOG(TAG, "write login info success ...");

    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }
    free(pack);

    return true;
}

//读取注册与登录成功与否
bool Client::ReadData(const char *name)
{
    int result = 0;
    int data;

    result = read(mSocketfd, &data, sizeof(int));
    if(!result)
    {
        LOGE(TAG, "read error ...");
        return false;
    }
    else
    {
        switch(data)
        {
            case RFAILED:
                system("clear");
                LOGE(TAG, "注册失败, 数据库操作失败或用户名已存在!!!");
                sleep(1);
                break;
            case RSUCCESSED:
                system("clear");
                LOG(TAG, "注册成功");
                sleep(1);
                break;
            case LFAILED:
                system("clear");
                LOG(TAG, "登录失败");
                sleep(1);
                return false;
            case LSUCCESSED:
                system("clear");
                LOG(TAG, "登录成功, 欢迎", name);
                sleep(1);
                break;
        }
    }
    
	return true;
}

void Client::RecvSignal(int sig)
{
    LOG(TAG, "recv signal is : ", sig);
}
