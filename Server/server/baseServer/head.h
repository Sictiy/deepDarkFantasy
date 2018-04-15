#ifndef __HEAD__
#define __HEAD__
#include <sys/socket.h>
#include <sys/epoll.h>
//#include <glog/logging.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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
#include <list>
#include <iomanip>
#include <vector>
#include <deque>
#include <bitset>
#include <algorithm>
#include <fstream>
#include <mysql.h>

#define MAXEVENTSIZE  1024
#define TIMEWAIT  1000
#define BUFFLEN  2048
#define HALLPORT 5050
#define DBPORT 5051
#define GMAEPORT 5052
#define USESTRING false
#define MAXLEN 1024*4
#define FPS 60

#define time_out 1

#define m_ip "127.0.0.1"
#define m_mysql_usr "root"
#define m_mysql_pw "123123"
#define m_database "deepdf"

#define fd_newconnect 0
#define breathe_cmd 7
#define c2s_rank_get  1 
#define s2c_rank_get  2
#define	h2d_get 3
#define	d2h_get 4
#define h2d_insert 5
#define d2h_insert 6


struct Msg{
	int fd;
	//int type;
	int cmd;
	char buff[MAXLEN];
};

struct RoleData{
	std::string name;
	uint score;
};
 
struct Connect{
	int fd;
	int count;
};
#endif
