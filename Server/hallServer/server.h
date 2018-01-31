#ifndef __SERVER__
#define __SERVER__

#include "head.h"

class Server
{
public:
	Server(Client * client);
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
	Client * ClientMgr;
	std::map<int,int> ClientMap;
};

#endif