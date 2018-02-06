#ifndef __HEAD__
#define __HEAD__
#include <sys/socket.h>
#include <sys/epoll.h>
//#include <glog/logging.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <map>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>
#include <mysql.h>

#define MAXEVENTSIZE  1024
#define TIMEWAIT  1000
#define BUFFLEN  2048
#define SERVERPORT 5050
#define USESTRING false
#define MAXLEN 1024*4

#define m_ip "127.0.0.1"
#define m_mysql_usr "root"
#define m_mysql_pw "qwer1234!"
#define m_database "deepdf"

#define c2s_rank_get  1
#define s2c_rank_get  2


class Client;

enum Cmd_id{
	Insert,
	Select
};

struct RoleData{
	std::string name;
	uint score;
};

struct Cmd{
	Client * client;
	Cmd_id cmd_id;
	RoleData data;
};

struct ClientRequest{
	int fd;
	Cmd_id cmd_id;
	char buff[MAXLEN];
};

struct Cli
{
	int fd;
	short offset;
	short length;
	char buff[MAXLEN];
};

/*typedef struct Respond{
	Client* client;
	Cmd_id cmd_id;
	std::vector<RoleData> vec;
}Respond;*/
 
#endif
