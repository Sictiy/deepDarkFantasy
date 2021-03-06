#ifndef __NETWORK__
#define __NETWORK__

#include "head.h"
#include "packet.h"
#include "msgmgr.h"
#include "time.h"

class Network{
public:
	Network();
	static Network &Instance(){
		static Network inst__;
		return inst__;
	}
	virtual ~Network();

	void run();
	bool createServer(const char* ip, int port);
	int connectServer(const char* ip, int port);

private:
	int epollLoop();
	int setNoblock(int fd);
	void ctlEvent(int fd, bool flag);
	void processTcpPackage(int fd);
	void processBreathe(const Msg &);
	void breathe();
	void newConnect(int fd);

private:
	int Epoolfd;
	int Listenfd;
	std::thread* Thread;
	int64_t Frame;
	std::map<int,Packet*> PacketMap;
	std::map<int,Connect> connections;
};

#define network Network::Instance()
#endif
