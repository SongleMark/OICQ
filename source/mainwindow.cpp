#include "../header/mainwindow.h"
#include <unistd.h>
#include "../header/logger.h"
#include "../header/client.h"
#define TAG "MainWindow"

MainWindow::MainWindow(Client *client, std::string main, std::string loginui, bool stop):
    mClient(client),
    mMainUi(main),
    mLoginUi(loginui),
    mIsStop(stop)
{

}

MainWindow::~MainWindow()
{
    delete mRegister;
}

void MainWindow::MainMenu()
{
    int select;
    mRegister = new RegisterServer(mClient);
    mLogin = new Login(mClient);
    while(!mIsStop)
    {
        system("clear");
        std::cout << mMainUi;
        LOGI(&select);
        switch(select)
        {
            case REGISTER://注册
                mRegister->InputInformation();
                break;
            case LOGIN://登录
                if(mLogin->UserSelect())
                {
                    //mName = mLogin->GetUserName();
                    strcpy(mName, mLogin->GetUserName());
                    LoginMenu();
                }
                break;
            case EXIT://其他选项则退出
            default:
                exit(0);
        }
    }
}

//登录后界面
void MainWindow::LoginMenu()
{
    int select;
    bool IsStop = false;
    OnlineUser *head = NULL;

    while(!IsStop)
    {
        system("clear");
        LOG(TAG, "\tWelCome, ", mName);
        std::cout << mLoginUi ;
        LOGI(&select);

        switch(select)
        {
        case GETUSER://获取在线用户链表
            //先发送获取在线用户链表的指令
            mClient->WriteData();
            //然后读取
            head = mClient->ReadOnlineUserNode();
            ShowOnlineUser(head);
            break;
        case SENDONE://向某个用户发送单个信息
            SendSignalMessage();
            break;
        case SENDALL://向所有在线用户发送信息
            //先发送获取在线用户链表的指令
            mClient->WriteData();
            //然后读取
            head = mClient->ReadOnlineUserNode();
            SendAllUserMessage(head);
            break;
        case RECVMESSAGE://接收消息
            RecvMessage();
            break;
        case EXIT:
        default:
            IsStop = true;
        }
    }
}

//打印在线用户信息
void MainWindow::ShowOnlineUser(OnlineUser *head)
{
    LOG(TAG, "start show user ...");
    if(NULL == head)
    {
        LOGE(TAG, "当前没有登录用户");
        return ;
    }
    
    int count = 1;
    OnlineUser *p = head;
    system("clear");
    std::cout << "\t\t\t**********************************************************\n";
    std::cout << "\t\t\t*                     Online User                        *\n";
    std::cout << "\t\t\t**********************************************************\n\n";
    std::cout << "\t\t\t*number       ID          name       port          ip    *\n\n";
    while(NULL != p)
    {
        std::cout << "\t\t\t*" << count << "            " << p->id << "           " << p->name 
                  << "        " << p->port << "      " << p->ip << "*\n\n";
        p = p->next;
        count ++;
    }
    std::cout << "\t\t\t**************************************\n";
    getchar();
    LOG(TAG, "please input any to continue ...");
    getchar();

}

//发送单个信息操作函数
void MainWindow::SendSignalMessage()
{
    //先发送获取在线用户链表的指令
    mClient->WriteData();
    //然后读取
    OnlineUser *head = mClient->ReadOnlineUserNode();
    ShowOnlineUser(head);

    if(NULL == head)
    {
        return ;
    }
    std::cout << "请选择接收者的ID：";
    std::cin >> mMessage.recvid;
    OnlineUser *p = head;
    while(p)
    {
        if(mMessage.recvid == p->id)
        {
            break;
        }
        p = p->next;
    }
    if(NULL == p)
    {
        LOGE(TAG, "input a unused id ...");
        return ;
    }
    mIp = p->ip;
    mPort = p->port;
    std::cout << "请输入发送的信息: ";
    std::cin >> mMessage.message;
    strcpy(mMessage.sendname ,mName);//发送者姓名
    strcpy(mMessage.recvname, p->name);//接收者姓名，在接受者回复时可用
    GetCurrentTime();

    mClient->WriteData(mMessage);
}

//向所有在线用户发送信息
void MainWindow::SendAllUserMessage(OnlineUser *head)
{
    if(NULL == head)
    {
        LOGE(TAG, "don't has online user ...");
        return ;
    }

    int count = 0;
    OnlineUser *p = head;
    OnlineUser *pfd = head;
    //先获取总共有多少在线用户
    while(NULL != p)
    {
        count ++;
        p = p->next;
    }
    Message message[count]; //待发送的消息结构体
    int i = 0;
    //获取时间
    time_t td = time(NULL);
    tm *current_time = localtime(&td);
    char str[32];
    strftime(str, 32, "%F-%T", current_time);
    while(NULL != pfd)
    {
        message[i].recvid = pfd->id; //接收者ID
        strcpy(message[i].recvname, pfd->name);//接收者姓名
        strcpy(message[i].time, str);//发送消息的时间
        i ++;
        pfd = pfd->next;
    }
    char mes[100];
    std::cout << "请输入发送的信息: ";
    std::cin >> mes;
    for(int i = 0; i < count ; i ++)
    {
        strcpy(message[i].message, mes); //将发送的消息
        strcpy(message[i].sendname, mName);//发送者姓名
    }
    
    for(int i = 0; i < count ; i ++)
    {
        mClient->WriteData(message[i]);
    }

}

//接收信息
void MainWindow::RecvMessage()
{
    //LOG(TAG, "start to client->readmessage ...");
    bool ret = mClient->ReadMessage();
    if(ret)
    {
        //LOG(TAG, "mClient->ReadMessage end ...");
    }
    else
    {
        LOG(TAG, "read message error ...");
        return;
    }
}

//获取发送消息的时间
void MainWindow::GetCurrentTime()
{
    time_t td = time(NULL);
    tm *current_time = localtime(&td);
    char str[32];
    strftime(str, 32, "%F-%T", current_time);
    strcpy(mMessage.time, str);
}

//线程同步接受信息--未使用
void *MainWindow::RecvThread(void *thread)
{
    MainWindow *recvtask = (MainWindow *)thread;
    recvtask->RecvMessage();
}