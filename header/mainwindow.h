#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <iostream>
#include "../header/register.h"
#include "../header/login.h"

#define MAINUI  "\t\t\t************************************\n"   \
                "\t\t\t*           局域网聊系统           *\n"   \
                "\t\t\t************************************\n\n" \
                "\t\t\t*          please input:           *\n\n" \
                "\t\t\t*           1.Register             *\n\n" \
                "\t\t\t*           2.Login                *\n\n" \
                "\t\t\t*           0.exit                 *\n\n" \
                "\t\t\t************************************\n\n"

#define LOGINUI "\t\t\t************************************\n"   \
                "\t\t\t*          please input:           *\n\n" \
                "\t\t\t*          1.get inline user       *\n\n" \
                "\t\t\t*          2.send one message      *\n\n" \
                "\t\t\t*          3.send all              *\n\n" \
                "\t\t\t*          4.recv message          *\n\n" \
                "\t\t\t*          0.exit                  *\n\n" \
                "\t\t\t************************************\n\n"

class MainWindow
{
public:
    explicit MainWindow(Client *client, std::string main = MAINUI, std::string loginui = LOGINUI, bool stop = false);
    virtual ~MainWindow();
    void MainMenu();
    void LoginMenu();
    void ShowOnlineUser(OnlineUser *head);
    void SendSignalMessage(OnlineUser *head);
    void RecvMessage();

private:
    std::string mMainUi;
    std::string mLoginUi;
    bool mIsStop;
    RegisterServer *mRegister;
    Login *mLogin;
    Client *mClient;
    char *mIp;
    unsigned short mPort;
    char mName[32];
    enum
    {
        EXIT,
        REGISTER,
        LOGIN
    };
    enum
    {
        GETUSER = 1,
        SENDONE,
        SENDALL,
        RECVMESSAGE       
    };
    Message mMessage;
    //Communication *mCommunication;
    
};

#endif
