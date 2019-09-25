#include "../header/server.h"
#include "../header/logger.h"
#include <cstring>

#define TAG "Server"

Server::Server(unsigned short port, bool stop, ClientNode *head, OnlineUser *uhead):
    mPort(port),
    mIsStop(stop),
    mHead(head),
    mUserhead(uhead)
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
    SetPort();
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

    socklen_t on = 1;
    //防止binderror
    result = setsockopt(mSocketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(result < 0)
    {
        LOG(TAG, "setsockopt error ...");
        return false;
    }
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

void Server::SetPort()
{
    char select[12];
    system("clear");
    LOG(TAG, "当前所有同网段IP可连，监听的端口号为8000, 是否更改端口号(yes/no)?");
    LOGI(select);
    if(0 == strcmp(select, "yes") || 0 == strcmp(select, "YES"))
    {
        LOG(TAG, "请输入新监听的端口号:");
        LOGI(&mPort);
    }
    else
    {
        LOG(TAG, "仍监听默认的ip地址和端口号");
        return ;
    }
    LOG(TAG, "当前监听的Port = ", mPort);
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
            if(result < 0)//此时移除下线节点
            {
                LOGE(TAG, "read error or TCP broken ...");
                RemoveUnlineUserNode(mTempfd);
                close(mTempfd);
                FD_CLR(mTempfd, &mSet);               
                break;
            }
            else if(result == 0)//此时移除下线节点
            {
                LOGE(TAG, "TCP broken ...");
                RemoveUnlineUserNode(mTempfd);
                close(mTempfd);
                FD_CLR(mTempfd, &mSet);                
                break;
            }

            int online = 0;
            bool IsBreak = false;
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
                    if(ret)//登录成功
                    {
                        WriteData(LSUCCESSED);
                        //登录成功则将该用户加入链表
                        CreateUserNode(mTempfd, mLogininfo.name);                       
                    }
                    else//登录失败
                    {
                        WriteData(LFAILED);
                    }
                        
                }
                else
                {
                    LOGE(TAG, "don't read mysql because of connect mysql error ...");
                }      
                break;
            case GETONLINEUSER://当包头表示获取在线用户链表时
                result = read(mTempfd, &online, sizeof(int));
                if(result < 0)
                {
                    LOGE(TAG, "read get online user commond error don't send online usernode...");
                    return false;
                }
                else
                {
                    LOG(TAG, "read commond = ", online);
                    //发送在线用户链表
                    WriteData();
                }          
                break;
            case MESSAGE://先接收信息在转发
                DealMessage();
                break;
            case ZERO:
                IsBreak = true;
                break;
            }
            if(IsBreak)
            {
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

//处理客户端发送的消息
bool Server::DealMessage()
{
    int result = 0;
    Message message;
    result = read(mTempfd, &message, sizeof(Message));
    if(result < 0)
    {
        LOGE(TAG, "read message error ...");
        return false;
    }
    else
    {
        LOG(TAG, "read message = ", message.message);
        LOG(TAG, "read recver fd = ", message.recvid);
        LOG(TAG, "send message time = ", message.time);
        LOG(TAG, "recv name = ", message.recvname);
        LOG(TAG, "send name = ", message.sendname);
        message.sendid = mTempfd;//将发送者的id给接受者，以便接收者回复
        Package *pack = (Package *)malloc(sizeof(Package) + sizeof(Message));
        Package temp;
        if(NULL == pack)
        {
            LOGE(TAG, "malloc error ...");
            return false;
        }
        pack->type = MESSAGE;
        pack->len = sizeof(Message);
        memcpy(pack->data, &message, pack->len);

        result = write(message.recvid, pack, sizeof(Package) + sizeof(Message));
        if(result < 0)
        {
            LOGE(TAG, "write error ...");
            return false;
        }      
        temp.type = ZERO;
        result = write(message.recvid, &temp, sizeof(Package));
        if(result < 0)
        {
            LOGE(TAG, "write error ...");
            return false;
        }

        free(pack);
    }
                
    LOG(TAG, "send message success ...");
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

//发送在线用户链表
bool Server::WriteData()
{
    int result = 0;
    OnlineUser *head = mUserhead;
    if(NULL == head)
    {
        LOGE(TAG, "don't has online user ...");
        return false;
    }
    Package *pack = (Package *)malloc(sizeof(Package) + sizeof(OnlineUser));
    if(NULL == pack)
    {
        LOGE(TAG, "malloc error ...");
        return false;
    }
    pack->type = GETONLINEUSER;
    pack->len = sizeof(Package) + sizeof(OnlineUser);

    while(NULL != head)
    {
        memcpy(pack->data, head, sizeof(OnlineUser));
        result = write(mTempfd, pack, pack->len);
        if(result < 0)
        {
            LOGE(TAG, "write error ...");
            return false;
        }
        head = head->next;
    }

    pack->type = ZERO;
    result = write(mTempfd, pack, sizeof(Package));
    if(result < 0)
    {
        LOGE(TAG, "write error ...");
        return false;
    }
    LOG(TAG, "write user node success ...");

    return true;
}

//从manager处获取数据库指针
void Server::GetStorageFromManager(StorageService *storage)
{
    mStorage = storage;
}

//创建在线用户链表
void Server::CreateUserNode(int id, char *name)
{
    OnlineUser *pnew = NULL;
    OnlineUser *last = NULL;

    pnew = (OnlineUser *)malloc(sizeof(OnlineUser));
    if(NULL == pnew)
    {
        LOGE(TAG, "malloc error");
        return ;
    }
    pnew->id = id;
    strcpy(pnew->name, name);
    strcpy(pnew->ip, inet_ntoa(mSaddr.sin_addr));
    pnew->port = mPort;
    pnew->next = NULL;
    
    //以尾插法创建链表
    if(NULL == mUserhead)
    {
        mUserhead = pnew;
    }
    else
    {
        last = mUserhead;
        while(NULL != last->next)
        {
            last = last->next;
        }
        last->next = pnew;
    }
    LOG(TAG, "create online user sucess ...");
    OnlineUser *p = mUserhead;
    std::cout << "id\tname\n";
    while(p)
    {
        std::cout << p->id << "\t" << p->name << std::endl;
        p = p->next;
    }
}

//移除下线节点
void Server::RemoveUnlineUserNode(int id)
{
    OnlineUser *pdel = mUserhead;
    OnlineUser *pre = mUserhead;
    if(id == mUserhead->id)
    {
        mUserhead = mUserhead->next;
        //free(mUserhead);
    }
    else
    {
        while(pdel)
        {
            if(id == pdel->id)
            {
                break;
            }
            pdel = pdel->next;
        }
        if(NULL == pdel)
        {
            LOGE(TAG, "the error is can't think about...");
            return ;
        }
        while(pre->next != pdel)
        {
            pre = pre->next;
        }
        pre->next = pdel->next;
        free(pdel);
    }

    OnlineUser *p = mUserhead;
    while(p)
    {
        std::cout << "name = " << p->name << "\tid = " << p->id << std::endl;
        p = p->next;
    }
    
}