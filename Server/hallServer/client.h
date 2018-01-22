#ifndef __CLIENT__
#define __CLIENT__


#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <map>


class Client 
{
public:
	Client(int fd);
	virtual ~Client();

	void disconnected();
	char* getBuff();
	void process();

private:
	void sendData(char* data);

private:
	char Buff[2048];
	int Fd;
};

#endif