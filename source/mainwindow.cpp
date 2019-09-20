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
            SendSignalMessage(head);
            break;
        case SENDALL://向所有在线用户发送信息
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
                  << "        " << p->port << "         " << p->ip << "\n\n";
        p = p->next;
        count ++;
    }
    std::cout << "\t\t\t**************************************\n";
    getchar();
    LOG(TAG, "please input any to continue ...");
    getchar();

}

//发送单个信息操作函数
void MainWindow::SendSignalMessage(OnlineUser *head)
{
    if(NULL == head)
    {
        LOG(TAG, "there don't have user ...");
        return ;
    }
    std::cout << "请选择接受者的ID：";
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
    LOG(TAG, "your send user ip = ", mIp);
    LOG(TAG, "your send user port = ", mPort);
    std::cout << "请输入发送的信息: " << std::endl;
    std::cin >> mMessage.message;
    mMessage.name = mName;

    mClient->WriteData(mMessage);
 #if 0   
    Communication  communication(mIp, mPort);
    if(communication.InitClient())
    {
        communication.SendSignalMessage(mMessage);
    }
#endif
}

//接收信息
void MainWindow::RecvMessage()
{
    LOG(TAG, "start to client->readmessage ...");
    mClient->ReadMessage();
    LOG(TAG, "mClient->ReadMessage end ...");
#if 0
    Communication communication("127.0.0.1", mClient->GetPort());
    if(communication.InitClient())
    {
        communication.RecvMessage();
    }
#endif
}