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
    SetIPandPort();
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

void Client::SetIPandPort()
{
    char select[12];
    char ip[20];
    system("clear");
    LOG(TAG, "默认连接的IPV4 = ", mIp);
    LOG(TAG, "默认连接的Port = ", mPort);
    LOG(TAG, "是否更改连接的ip及port(yes/no)?");
    LOGI(select);
    if(0 == strcmp(select, "yes") || 0 == strcmp(select, "YES"))
    {
        LOG(TAG, "请输入新IPV4：");
        LOGI(ip);
        //mIp = ip;
        mIp = new char[20];
        strcpy(mIp, ip);
        LOG(TAG, "请输入新PORT: ");
        LOGI(&mPort);
    }
    else
    {
        LOG(TAG, "连接默认的ip地址和端口");
        sleep(1);
        return ;
    }
    LOG(TAG, "当前连接的IPV4 = ", mIp);
    LOG(TAG, "当前连接的Port = ", mPort); 
    sleep(1);
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

//向服务器获取在线用户链表
bool Client::WriteData()
{
    int result = 0;
    int online = 1;
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(int));
    if(NULL == pack)
    {
        LOGE(TAG, "malloc error ...");
        return false;
    }
    pack->type = GETONLINEUSER;
    pack->len = sizeof(Package) + sizeof(int);
    memcpy(pack->data, &online, sizeof(int));

    result = write(mSocketfd, pack, pack->len);
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }

    pack->type = ZERO;
    result = write(mSocketfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }
    LOG(TAG, "send get online user commond success ..."); 
    free(pack);

    return true;
}

//向服务器发送用户输入的信息
bool Client::WriteData(Message message)
{
    int result = 0;
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(Message));
    if(NULL == pack)
    {
        LOGE(TAG, "malloc error ...");
        return false;
    }
    pack->type = MESSAGE;
    pack->len = sizeof(Package) + sizeof(Message);
    memcpy(pack->data, &message, sizeof(Message));

    result = write(mSocketfd, pack, pack->len);
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }

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

//读取注册/登录成功与否
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
                std::cout << RED ;               
                LOG(TAG, "登录失败");
                std::cout << SRC;
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

//从服务器读取用户发送的信息
bool Client::ReadMessage()
{
    LOG(TAG, "this is ReadMessage() ...");
    Message message ;
    Package *pack = (Package *)malloc(sizeof(Package));
    bool IsStop = false;
    int result = 0;

    LOG(TAG, "start to read ...");
    while(!IsStop)
    {
        LOG(TAG, "read pack_head ...");
        result = read(mSocketfd, pack, sizeof(Package));
        if(result < 0)
        {
            LOGE(TAG, "read error ...");
            perror("read:");
            return false;
        }
        else if(0 == result)
        {
            LOG(TAG, "read over ...");
            IsStop = true;
        }
        if(pack->type == ZERO)
        {
            IsStop = true;
        }
        else if(pack->type == MESSAGE)
        {
            LOG(TAG, "read message ...");
            result = read(mSocketfd, &message, pack->len);
            LOG(TAG, "read message over ...");
            if(result < 0)
            {
                LOGE(TAG, "read error ...");
                perror("read:");
                return false;
            }
            std::cout << "id:" << message.sendid << "\tname:" << message.name << ": \n";
            std::cout << message.message << std::endl; 
        }
        else
        {
            IsStop = true;
        } 
    }
    
    return true; 
}

//读取从服务器发送过来的在线用户链表
OnlineUser *Client::ReadOnlineUserNode()
{
    OnlineUser *head = NULL;
    OnlineUser *pnew = NULL;
    OnlineUser *last = NULL;
    int result = 0;
    bool IsStop = false;
    Package pack;

    while(!IsStop)
    {
        result = read(mSocketfd, &pack, sizeof(Package));
        if(result < 0)
        {
            LOGE(TAG, "read error ...");
            return head;
        }
        else if(0 == result)
        {
            IsStop = true;
        }
        if(pack.type == ZERO)
        {
            LOG(TAG, "read over ...");
            IsStop = true;
        }
        else if(pack.type == GETONLINEUSER)
        {
            pnew = (OnlineUser *)malloc(sizeof(OnlineUser));
            if(NULL == pnew)
            {
                LOGE(TAG, "malloc error ...");
                return head;
            }
            int ret = read(mSocketfd, pnew, sizeof(OnlineUser));
            if(ret < 0)
            {
                LOGE(TAG, "read error ...");
                return head;
            }
            pnew->next = head;
            head = pnew;
            #if 0
            if(NULL == head)
            {
                head = pnew;
            }
            else
            {
                last = head;
                while(NULL != last->next)
                {
                    last = last->next;
                }
                last->next = pnew;
            }  
            #endif   
        }
        else
        {
            LOG(TAG, "read over ...");
            IsStop = true;    
        }      
    }

    return head;
}

void Client::RecvSignal(int sig)
{
    LOG(TAG, "recv signal is : ", sig);
}

char *Client::GetAddress()
{
    return mIp;
}

unsigned short Client::GetPort()
{
    return mPort;
}