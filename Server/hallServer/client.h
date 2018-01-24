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
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <map>
#include <vector>
#include<algorithm>

#define USESTRING true

typedef struct RoleData{
	char name[20];
	uint score;
}RoleData;

class Client 
{
public:
	Client(int fd);
	virtual ~Client();

	void disconnected();
	char* getBuff();
	void process();

private:
	void sendData(std::string data);
	void selectRole();
	void createRole();
	void getDataFromDB();
	void processDBData();

private:
	char Buff[2048];
	int Fd;
	std::vector<RoleData> RoleDataList;
};

#endif