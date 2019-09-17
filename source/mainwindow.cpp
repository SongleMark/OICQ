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
            case REGISTER:
                mRegister->InputInformation();
                break;
            case LOGIN:
                if(mLogin->UserSelect())
                    LoginMenu();
                break;
            case EXIT:
            default:
                exit(0);
        }
    }
}

void MainWindow::LoginMenu()
{
    int select;
    bool IsStop = false;
    while(!IsStop)
    {
        system("clear");
        std::cout << mLoginUi ;
        LOGI(&select);

        switch(select)
        {
        case GETUSER:
            break;
        case SENDONE:
            break;
        case SENDALL:
            break;
        case EXIT:
        default:
            IsStop = true;
        }
    }
}