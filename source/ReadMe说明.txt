1.运行server为服务器  运行client为客户端
2.用make即可编译
3.controlclient.cpp为client的main函数文件，controlserver.cpp为server的main文件
4.logger.h logger.cpp 为打印日志文件

注意:
1.发送在线链表必须打包发送，否则接受不到或卡死，write的数据大小不能出错，否则也会卡死