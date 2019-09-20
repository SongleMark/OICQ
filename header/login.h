#ifndef LOGIN_H
#define LOGIN_H
#include "../header/client.h"

class Login
{
public:
    explicit Login(Client *client);
    virtual ~Login();
    bool  UserSelect();
    char *GetUserName();

private:
    Client *mClient;
    char mName[32];
};

#endif //login.h