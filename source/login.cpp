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

//用户选择登录时操作
bool Login::UserSelect()
{
    char *password;
    LoginInfo logininfo;
    bool result = true;

    LOGI(TAG, "please input username : ", logininfo.name);
    LOGI(TAG, "please input password : ", logininfo.password);
    //mName = logininfo.name;
    strcpy(mName, logininfo.name);
    password = logininfo.password;
    //将输入的用户名和密码发送给服务器处理
    result = mClient->WriteData(logininfo);
    if(!result)
    {
        LOGE(TAG, "write login info failed ...");
        return false;
    }
    //获取是否登录成功
    result = mClient->ReadData(mName);
    if(!result)
    {
        LOGE(TAG, "login failed ...");
        return false;
    }

    return true;
}

//给外界提供登录的用户名
char *Login::GetUserName()
{
    return mName;
}