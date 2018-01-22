#ifndef __SERVER__
#define __SERVER__

#include <thread>
#include <map>

class Client;

class Server
{
public:
	Server();
	virtual ~Server();

	bool createServer();
	void run();

private:
	int epollLoop();
	int setNoblock();
	void ctlEvent();

private:
	int Epoolfd;
	int Listenfd;
	bool Isloop;
	std::thread* Thread;
	int64_t Frame;

	std::map<int,Client*> ClientMap;
};

#endif