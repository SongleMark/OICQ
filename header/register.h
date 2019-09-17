#ifndef REGISTER_H
#define REGISTER_H
#include <iostream>
#include "../header/client.h"

class RegisterServer
{
public:
    explicit RegisterServer(Client *client, int id = 0, int64_t tel = 110, char *name = (char *)"");
    virtual ~RegisterServer();
    bool InputInformation();
    int64_t GetTelphone();
    std::string GetName();

private:
    int mId;
    int64_t mTel;
    char *mName;
    char *mPassword; 
    RegisteInfo mRegister; 
    Client *mClient;
    
};

#endif