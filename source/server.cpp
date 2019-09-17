#include "../header/server.h"
#include "../header/logger.h"
#include <cstring>

#define TAG "Server"

Server::Server(unsigned short port, bool stop, ClientNode *head):
    mPort(port),
    mIsStop(stop),
    mHead(head)
{

}

Server::~Server()
{

}

//初始化socket套接字 bind() listen()
bool Server::Init()
{
    int result = 0;
    const int BackLog = 20;
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
    mSaddr.sin_addr.s_addr = INADDR_ANY;
    LOG(TAG, "bind ...");
    result = bind(mSocketfd, (sockaddr *)&mSaddr, (socklen_t)sizeof(mSaddr));
    if(result < 0)
    {
        LOGE(TAG, "bind failed ...");
        return false;
    }

    LOG(TAG, "listen ...");
    result = listen(mSocketfd, BackLog);
    if(result < 0)
    {
        LOGE(TAG, "listen railed ...");
        return false;
    }

    return true;
}

void Server::DeInit()
{

}

//服务器等待连接客户端，在main函数调用此函数即可
bool Server::Connect()
{
    bool result = true;
    void *exit;
    result = Init();
    if(!result)
    {
        LOGE(TAG, "init error ...");
        return false;
    }
    //线程---由于selectclient需要循环操作，所以单独开辟线程提高效率
    if(0 != pthread_create(&mPthread, NULL, Thread, this))
    {
        LOGE(TAG, "create thread error ...");
        return false;
    }
    if(0 != pthread_join(mPthread, &exit))
    {
        LOGE(TAG, "wait the thread error ...");
        return false;
    }

    return true;
}

//服务器断开连接
void Server::DisConnect()
{
    close(mSocketfd);
	close(mTempfd);
	FD_CLR(mTempfd, &mSet);
	FD_ZERO(&mSet);
}

//服务器select客户端，创建客户端链表，并等待接受信息和发送信息
bool Server::SelectClient()
{
    int result = 0;
    //先清空集合，再将套接字加入集合
    FD_ZERO(&mSet);
    FD_SET(mSocketfd, &mSet);
    mMaxfd = mSocketfd;

    while(!mIsStop)
    {
        mReadset = mSet;
        LOG(TAG, "select ...");
        result = select(mMaxfd + 1, &mReadset, NULL, NULL, NULL);
        if(result < 0)
        {
            LOGE(TAG, "select failed ...");
            return false;
        }
        //如果该套接字在此集合内则进行accept操作，并创建客户端链表
        if(FD_ISSET(mSocketfd, &mReadset))
        {
            mAddrlen = sizeof(mCaddr);
            LOG(TAG, "accept ...");
            mAcceptfd = accept(mSocketfd, (sockaddr *)&mSaddr, (socklen_t *)&mAddrlen);
            if(mAcceptfd < 0)
            {
                LOGE(TAG, "accept failed ...");
                return false;
            }
            LOG(TAG, "client ip = ", inet_ntoa(mSaddr.sin_addr));
            LOG(TAG, "client port = ", ntohs(mSaddr.sin_port));
            //将accept返回的描述符加入集合
            FD_SET(mAcceptfd, &mSet);
            if(mMaxfd < mAcceptfd)
            {
                mMaxfd = mAcceptfd;
            }
            //以头插法创建客户端链表
            mNode = (ClientNode *)malloc(sizeof(ClientNode));
            if(!mNode)
            {
                LOGE(TAG, "malloc failed ...");
                return false;
            }
            mNode->fd = mAcceptfd;
            mNode->caddr = mCaddr;
            mNode->next = mHead;
            mHead = mNode;
        }
        //遍历客户端链表
        for(mNode = mHead ; mNode != NULL; mNode = mNode->next)
        {
            mTempfd = mNode->fd;
            if(!FD_ISSET(mTempfd, &mReadset))
            {
                continue;
            }
            //开始读取客户端信息,拆包判断
            Package pack;
            result = read(mTempfd, &pack, sizeof(Package));
            if(result < 0)
            {
                LOGE(TAG, "read error or TCP broken ...");
                close(mTempfd);
                FD_CLR(mTempfd, &mSet);
                break;
            }
            else if(result == 0)
            {
                LOGE(TAG, "TCP broken ...");
                close(mTempfd);
                FD_CLR(mTempfd, &mSet);
                break;
            }

            switch(pack.type)
            {
            case REGISTER://当包头表示注册信息时
                result = read(mTempfd, &mRegistinfo, sizeof(RegisteInfo));
                if(result < 0)
                {
                    LOGE(TAG, "read error ...");
                    return false;
                }
                LOG(TAG, "read your name = ", mRegistinfo.name);
                LOG(TAG, "read telphone  = ", mRegistinfo.telphone);
                LOG(TAG, "read password  = ", mRegistinfo.password);
                LOG(TAG, "and your id is : ", mTempfd); 
                mRegistinfo.id = mTempfd;
                if(mStorage)
                {
                    bool ret = mStorage->WriteToMysql(mRegistinfo);
                    if( !ret )
                    {
                        WriteData(RFAILED);
                    }
                    else
                    {
                        WriteData(RSUCCESSED);
                    }
                        
                }
                else
                {
                    LOGE(TAG, "don't save to mysql because of connect mysql error ...");
                }                    
                break;
            case LOGIN://当包头表示登录信息时
                result = read(mTempfd, &mLogininfo, sizeof(LoginInfo));
                if(result < 0)
                {
                    LOGE(TAG, "read error ...");
                    return false;
                }
                LOG(TAG, "Login username = ", mLogininfo.name);
                LOG(TAG, "Login password = ", mLogininfo.password);
                if(mStorage)
                {
                    bool ret = mStorage->ReadFromMysql(mLogininfo.name, mLogininfo.password);
                    if(ret)
                    {
                        //LOG(TAG, "login success ...");
                        WriteData(LSUCCESSED);
                    }
                    else
                    {
                        //LOG(TAG, "login failed ...");
                        WriteData(LFAILED);
                    }
                        
                }
                else
                {
                    LOGE(TAG, "don't read mysql because of connect mysql error ...");
                }      
                break;
            case ZERO:
                break;
            }
        }        
    }

    return true;
}

void Server::RecvSignal(int sig)
{
    LOG(TAG, "recv signal is : ", sig);
}

//线程---由于selectclient需要循环操作，所以单独开辟线程
void *Server::Thread(void *task)
{
    bool result = true;
    Server *thread = (Server *)task;
    result = thread->SelectClient();
    if(!result)
    {
        LOGE(TAG, "select client failed ...");
    }
}

bool Server::ReadData()
{
    int result = 0;
	return true;
}

//向客户端写入注册/登录成功与否信息
bool Server::WriteData(const int data)
{
    int result = 0;
    result = write(mTempfd, &data, sizeof(int));
    if(result < 0)
    {
        LOGE(TAG, "write data error ...");
    }
    LOG(TAG, "write success data = ", data);

	return true;
}

//从manager处获取数据库指针
void Server::GetStorageFromManager(StorageService *storage)
{
    mStorage = storage;
}