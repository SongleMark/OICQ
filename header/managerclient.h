#ifndef MANAGERCLIENT_H
#define MANAGERCLIENT_H
#include "client.h"
#include "mainwindow.h"

class ManagerService
{
public:
    explicit ManagerService();
    virtual ~ManagerService();

private:
    Client *mClient;
    MainWindow *mMainwindow;
};

#endif