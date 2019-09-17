#include "../header/login.h"
#include "../header/logger.h"

#define TAG "Login"

Login::Login(Client *client):
    mClient(client)
{

}

Login::~Login()
{

}

bool Login::UserSelect()
{
    char *name;
    char *password;
    LoginInfo logininfo;
    bool result = true;

    LOGI(TAG, "please input username : ", logininfo.name);
    LOGI(TAG, "please input password : ", logininfo.password);
    name = logininfo.name;
    password = logininfo.password;
    result = mClient->WriteData(logininfo);
    if(!result)
    {
        LOGE(TAG, "write login info failed ...");
        return false;
    }
    result = mClient->ReadData(name);
    if(!result)
    {
        LOGE(TAG, "login failed ...");
        return false;
    }

    return true;
}