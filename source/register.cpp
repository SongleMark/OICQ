#include "../header/register.h"
#include "../header/logger.h"
#define TAG "RegisterServer"

RegisterServer::RegisterServer(Client *client, int id, int64_t tel, char *name):
    mClient(client),
    mId(id),
    mTel(tel),
    mName(name)
{

}

RegisterServer::~RegisterServer()
{

}

//用户输入注册信息并且将该信息发送给服务器处理
bool RegisterServer::InputInformation()
{  
    int count = 1;
    bool IsStop = false;
    char *password;
    LOGI(TAG, "please input your name: ", mRegister.name);
    LOGI(TAG, "please input your tel : ", &mTel);
    while(!IsStop)
    {
        LOGI(TAG, "please input password : ", mRegister.password);
        LOGI(TAG, "input password again  : ", password);
        if(0 == strcmp(password, mRegister.password))
        {
            break;
        }
        else
        {
            LOGE(TAG, "please enter the correct password...");
            if(count >= 3)
            {
                LOGE(TAG, "too many error's when enter password, don't register...");
                sleep(1);
                return false;
            }
            count ++;
        }
    }
    mName = mRegister.name;
    mRegister.telphone = mTel;
    mPassword = mRegister.password; 
    bool ret = mClient->WriteData(mRegister);
    if(!ret)
    {
        LOGE(TAG, "write regist info failed ...");
        return false;
    }
    mClient->ReadData();

    return true;
}

int64_t RegisterServer::GetTelphone()
{
    return mTel;
}

std::string RegisterServer::GetName()
{
    return mName;
}