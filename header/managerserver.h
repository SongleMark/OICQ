#ifndef MANAGERSERVER_H
#define MANAGERSERVER_H
#include "server.h"
#include "storage.h"

class ManagerService
{
public:
    explicit ManagerService();
    virtual ~ManagerService();

private:
    Server *mServer;
    StorageService *mStorage;
};

#endif