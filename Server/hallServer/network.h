#ifndef __NETWORK__
#define __NETWORK__

#include "head.h"

class Network
{
public:
	Network(Client * client);
	virtual ~Network();

	void run();
	void setCliMgr(Client* client);

private:
	int epollLoop();
	bool createServer(const char* ip, int port);
	int setNoblock(int fd);
	void ctlEvent(int fd, bool flag);
	void processTcpPackage(int fd);

private:
	int Epoolfd;
	int Listenfd;
	bool Isloop;
	std::thread* Thread;
	int64_t Frame;
	Client * ClientMgr;
	std::map<int,Cli> ClientMap;
};

#endif