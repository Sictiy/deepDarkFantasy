#ifndef __SERVER__
#define __SERVER__

#include "dbmgr.h"
#include "client.h"

#include <sys/socket.h>
#include <sys/epoll.h>
//#include <glog/logging.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <csignal>
#include <iostream>
#include <thread>
#include <map>

#define MAXEVENTSIZE  1024
#define TIMEWAIT  1000
#define BUFFLEN  2014
#define SERVERPORT 8080


class Client;

class Server
{
public:
	Server();
	virtual ~Server();

	bool init();
	void run();

private:
	int epollLoop();
	bool createServer(const char* ip, int port);
	int setNoblock(int fd);
	void ctlEvent(int fd, bool flag);
	void setLog();
	void setDaemon();

private:
	int Epoolfd;
	int Listenfd;
	bool Isloop;
	std::thread* Thread;
	int64_t Frame;

	std::map<int,Client*> ClientMap;
};

#endif
