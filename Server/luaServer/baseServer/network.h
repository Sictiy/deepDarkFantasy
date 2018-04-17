#ifndef __NETWORK__
#define __NETWORK__

#include "head.h"
#include "packet.h"
#include "luamgr.h"
// #include "msgmgr.h"
#include "time.h"

class Network{
public:
	Network();
	static Network* Instance();
	// static Network &Instance(){
	// 	static Network inst__;
	// 	return inst__;
	// }
	virtual ~Network();

	void run();
	bool init();
	bool createServer(const char* ip, int port);
	int connectServer(const char* ip, int port);
	Packet* getPacket(int fd);
	Packet* getListen();
	void breathe(int fps);

private:
	int epollLoop();
	int setNoblock(int fd);
	void ctlEvent(int fd, bool flag, PacketType type);
	void processTcpPackage(int fd);
	void processBreathe(Packet * packet);
	void newConnect(int fd);

private:
	int Epoolfd;
	int Listenfd;
	std::thread* Thread;
	int64_t Frame;
	std::map<int,Packet*> PacketMap;
};

// #define network Network::Instance()
#endif
