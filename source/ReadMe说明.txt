项目简介:
[局域网聊天系统]：
1.首先运行server可选择监听的端口号，运行client可选择连接的ip地址和端口号，连接上后可选择注册/登录;
2.注册的用户名/手机号码不能重复，否则会提示注册失败，登录成功后可查看当前在线用户以及发送单个信息和群发信息

[配置]:
1.linux系统，mysql数据库
2.在数据库中建一个库名为:OICQ
可执行以下语句:
1. mysql -uroot -p 然后输入密码
2. CREATE DATABASE `OICQ` CHARACTER SET utf8 COLLATE utf8_general_ci;


[项目文件简介]:
1.用make即可编译生成server服务器和client客户端两个可执行文件
2.运行server为服务器  运行client为客户端
3.controlclient.cpp为client的main函数文件，controlserver.cpp为server的main文件
4.logger.h logger.cpp 为打印日志文件
5.MainWindow.h/.cpp 为命令行界面文件
6.login.h/.cpp为登录类
7.register.h/.cpp为注册类
8.storage.h/.cpp为数据库存储类


注意:
1.发送在线链表必须打包发送，否则接受不到或卡死，write的数据大小不能出错，否则也会卡死
2.在使用char *类型时, 若要赋值应避免出现段错误, 要么使用char var[NUM], strcpy()