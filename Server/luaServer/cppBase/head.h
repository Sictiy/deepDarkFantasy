#ifndef __HEAD__
#define __HEAD__
#include "PlatformLinux.h"
#include "ToolMacros.h"
// #include "ToolFunctions.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <getopt.h>
#include <dirent.h>
// #include <uuid/uuid.h>
//#include <glog/logging.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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
#include <cstdint>
#include <assert.h>
#include <float.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <mutex>
#include <mysql.h>
#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>
#include <Luna.h>

// typedef int BOOL;
// typedef unsigned int DWORD;
// typedef unsigned short WORD;
// typedef unsigned char BYTE;
// typedef double float64_t
// typedef unsigned long u_long;
// typedef unsigned long ULONG;
// typedef void* HANDLE;
// typedef void* HMODULE;
// typedef long HRESULT;

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

struct callStruct{
	int id;
	size_t dataLen;
	unsigned char data[1];
	/* data */
};

#endif
