#ifndef __NETWORK__
#define __NETWORK__

#include "head.h"
#include "packet.h"

class Network{
public:
	Network();
	virtual ~Network();

	void run();
	void setCliMgr(Client* client);
	bool createServer(const char* ip, int port);

private:
	int epollLoop();
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
	std::map<int,Packet*> PacketMap;
	//std::map<int,Cli> ClientMap;
};

#endif