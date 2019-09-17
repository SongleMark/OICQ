#include "../header/managerclient.h"
#include "../header/logger.h"
#define TAG "Manager"

ManagerService::ManagerService()
{
    mClient = new Client();
    if(!mClient->Connect())
    {
        LOGE(TAG, "connect server error ...");
        return ;
    }
    mMainwindow = new MainWindow(mClient);   
    mMainwindow->MainMenu();

}

ManagerService::~ManagerService()
{
    delete mClient;
    delete mMainwindow;
}