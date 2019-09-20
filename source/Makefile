#Makefile文件
#同时生成client server两个可执行文件

CC 		    = gcc
CXX		    = g++
INCLUDEPATH = -I../header
FLAG		= -g  -o
CLIENT		= client.o \
			  controlclient.o \
			  logger.o \
			  register.o \
			  mainwindow.o \
			  managerclient.o \
			  login.o
SERVER		= server.o \
			  controlserver.o \
			  logger.o \
			  storage.o \
			  managerserver.o
COBJECT		= $(CLIENT) -o  client
SOBJECT		= $(SERVER) -o  server
TARGET		= client server
LIB			= -lpthread -lmysqlclient
CLEAN		= rm *.o

all:$(TARGET)

#create client
client:$(CLIENT)
	$(CXX)  $(COBJECT) 	   #生成client可执行文件

client.o:client.cpp 
	$(CXX) -c client.cpp $(INCLUDEPATH)  $(FLAG) client.o 

controlclient.o:controlclient.cpp 
	$(CXX) -c controlclient.cpp $(INCLUDEPATH) $(FLAG) controlclient.o

register.o:register.cpp
	$(CXX) -c register.cpp $(INCLUDEPATH) $(FLAG) register.o

mainwindow.o:mainwindow.cpp
	$(CXX) -c mainwindow.cpp $(INCLUDEPATH) $(FLAG) mainwindow.o

managerclient.o:managerclient.cpp
	$(CXX) -c managerclient.cpp $(INCLUDEPATH) $(FLAG) managerclient.o

login.o:login.cpp
	$(CXX) -c login.cpp $(INCLUDEPATH) $(FLAG) login.o

logger.o:logger.cpp
	$(CXX) -c logger.cpp $(INCLUDEPATH) $(FLAG) logger.o

#create server
server:$(SERVER)
	$(CXX) $(SOBJECT) $(LIB)         #生成server可执行文件

server.o:server.cpp
	$(CXX) -c server.cpp $(INCLUDEPATH) $(FLAG) server.o

controlserver.o:controlserver.cpp
	$(CXX) -c controlserver.cpp $(INCLUDEPATH) $(FLAG) controlserver.o

storage.o:storage.cpp
	$(CXX) -c storage.cpp $(INCLUDEPATH) $(FLAG) storage.o

managerserver.o:managerserver.cpp
	$(CXX) -c managerserver.cpp $(INCLUDEPATH) $(FLAG) managerserver.o

clean:
	$(CLEAN)

