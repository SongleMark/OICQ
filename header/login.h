#ifndef LOGIN_H
#define LOGIN_H
#include "../header/client.h"

class Login
{
public:
    explicit Login(Client *client);
    virtual ~Login();
    bool  UserSelect();

private:
    Client *mClient;
};

#endif //login.h