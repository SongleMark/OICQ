#ifndef STORAGE_H
#define STORAGE_H
#include <cstdio>
#include <iostream>
#include <mysql/mysql.h>

struct RegisteInfo
{
    int id;
    int64_t telphone;
    char name[32];
    char password[32];
};

struct LoginInfo
{
	char name[66];
	char password[66];
};

class StorageService
{
public: 
    virtual ~StorageService();
    static StorageService *mInstane;
    static StorageService *GetInstance();
    bool Connect();
    void DisConnect();
    bool WriteToMysql(const RegisteInfo &registinfo);
    bool ReadFromMysql(const char *name, const char *password);

private:
    explicit StorageService(char *db = (char *)"TCP", char *user = (char *)"root", \
                            char *host = (char *)"localhost", char *password = (char *)"123456");
    MYSQL *mMysql;     /*数据库句柄                      */
    MYSQL_RES *mResult;/*结果集                         */
    MYSQL_ROW mRow;    /*row[i]可用于输出该行第i个字段的数据*/
    char *mDatabase;   /*数据库名称                      */
    char *mUser;       /*用户名                         */
    char *mPassword;   /*密码                           */
    char *mHostname;   /*数据库地址                      */

};

#endif