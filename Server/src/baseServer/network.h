#ifndef __NETWORK__
#define __NETWORK__

#include "head.h"
#include "packet.h"
#include "msgmgr.h"

class Network{
public:
	Network();
	virtual ~Network();

	void run();
	bool createServer(const char* ip, int port);
	int connectServer(const char* ip, int port);

private:
	int epollLoop();
	int setNoblock(int fd);
	void ctlEvent(int fd, bool flag);
	void processTcpPackage(int fd);

private:
	int Epoolfd;
	int Listenfd;
	std::thread* Thread;
	int64_t Frame;
	std::map<int,Packet*> PacketMap;
};

#endif