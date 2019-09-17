#include "../header/managerserver.h"
#include "../header/logger.h"
#define TAG "Manager"

ManagerService::ManagerService()
{
    bool result = true;
    mServer = new Server();
    mStorage = StorageService::GetInstance();

    result = mStorage->Connect();
    if(result)
    {
        mServer->GetStorageFromManager(mStorage);
    }
    else
    {
        LOGE(TAG, "connect mysql error ...");
    }
    result = mServer->Connect();
    if(!result)
    {
        LOGE(TAG, "connect server failed ...");
        return ;
    }
}

ManagerService::~ManagerService()
{
    mStorage->DisConnect();
    delete mStorage;
    delete mServer;
}