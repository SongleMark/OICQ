#include "../header/storage.h"
#include "../header/logger.h"
#include <cstring>
#define TAG  "StorageService"
#define UTF8 "utf8"

StorageService *StorageService::mInstane = new StorageService();

StorageService *StorageService::GetInstance()
{
    return mInstane;
}

StorageService::StorageService(char *db, char *user, char *host, char *password):
    mDatabase(db),
    mUser(user),
    mHostname(host),
    mPassword(password)
{
}

StorageService::~StorageService()
{
    delete mInstane;
}
#if 0
MYSQL *		STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned long clientflag);
#endif
//连接数据库
bool StorageService::Connect()
{
    //数据库初始化
    mMysql = mysql_init(NULL);
    if(NULL == mMysql)
    {
        LOGE(TAG, "mysql_init error ...");
        return false;
    }
    //连接数据库
    if(!mysql_real_connect(mMysql, mHostname, mUser, mPassword, mDatabase, 0, NULL, 0))
    {
        LOGE(TAG, "connect mysql error may not has the database...");
        mysql_error(mMysql);
        return false;
    }
    //设置数据库字符集
    mysql_set_character_set(mMysql, UTF8);

    return true;
}

//断开与数据库的连接
void StorageService::DisConnect()
{
    mysql_free_result(mResult);//释放结果集
    mysql_close(mMysql);       //关闭数据库
}

//向数据库写入注册信息
bool StorageService::WriteToMysql(const RegisteInfo &registinfo)
{
    int result = 0;
    //[如果不存在regist表则创建该表]
    const char *sql_creat = "CREATE TABLE IF NOT EXISTS regist(id int not null default 0, \
                             name char(32) not null default 'ma', \
                             tel bigint primary key not null default 110, \
                             password char(32) not null default '123abc');";
    //LOG(TAG, sql_creat);
    result = mysql_query(mMysql, sql_creat); //mysql执行语句
    if(result < 0)
    {
        LOGE(TAG, "create table error ...");
        return false;
    }
    //[检查mysql中是否已存在该用户名]
    char sql_query[66];
    sprintf(sql_query, "select count(*) from regist where name='%s';", registinfo.name);
    LOG(TAG, sql_query);
    result = mysql_query(mMysql, sql_query);
    if(result < 0)
    {
        LOGE(TAG, "mysql_query error ...");
        mysql_error(mMysql);
        return false;
    }
    //获取检索结果
    mResult = mysql_store_result(mMysql);
    if(NULL == mResult)
    {
        mysql_error(mMysql);
        LOGE(TAG, "get mysql result error ...");
        return false;
    }
    //比对输入的用户名
    while(NULL != (mRow = mysql_fetch_row(mResult)))
    {
        LOG(TAG, " name number = ", mRow[0]);
        if(0 == strcmp(mRow[0], "1"))
        {
            LOGE(TAG, "该名称已注册...");
            return false;
        }
    }

    //[向表中插入数据]
    char sql_insert[88] ; //注意不能写成char *sql_insert否则sprintf会报段错误
    sprintf(sql_insert, "INSERT INTO regist values(%d, '%s', %ld, '%s');",
           registinfo.id, registinfo.name, registinfo.telphone, registinfo.password);
    LOG(TAG, sql_insert);
    result = mysql_query(mMysql, sql_insert);
    if(result < 0)
    {
        LOGE(TAG, "insert values failed ...");
        return false;
    }

    mysql_free_result(mResult);//释放结果集
    return true;
}

//根据输入的用户名密码读取数据库信息
bool  StorageService::ReadFromMysql(const char *name, const char *password)
{
    int result = 0;
    char sql_query[66];
    sprintf(sql_query, "select password from regist where name='%s';", name);
    LOG(TAG, sql_query);
    result = mysql_query(mMysql, sql_query);
    if(0 != result)
    {
        LOGE(TAG, "query mysql error ...");
        return false;
    }

    //获取检索结果
    mResult = mysql_store_result(mMysql);
    if(NULL == mResult)
    {
        LOGE(TAG, "get mysql result error ...");
        return false;
    }
    
    //比对输入的密码
    while(NULL != (mRow = mysql_fetch_row(mResult)))
    {
        LOG(TAG, "PASSWORD = ", mRow[0]);
        if(0 == strcmp(mRow[0], password))
        {
            return true;
        }
    }

    mysql_free_result(mResult);//释放结果集
    return false;
}