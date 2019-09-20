#include "../header/chart.h"
#include "../header/logger.h"
#define TAG "Chart"

Chart::Chart(char *ip, unsigned short port)
{

}

Chart::~Chart()
{

}

bool Chart::Init()
{
    mSockaddrfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(mSockaddrfd < 0)
    {
        LOGE(TAG, "socket error ...");
        return false;
    }
    mSaddr.sin_family = AF_INET;
    mSaddr.sin_port = htons(mPort);
    mSaddr.sin_addr.s_addr = inet_addr(mIp);
}