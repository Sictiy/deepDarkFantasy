// #ifndef __SERVER__
// #define __SERVER__
#pragma once

#include "head.h"

class Server{
public:
	Server();
	virtual ~Server();

	bool init();
	void run();

	static Server* Instance();
	bool isQuit();
	//bool createServer(const char* ip, int port);

private:
	//void setLog();
	void setDaemon();
	void registerHandler();
	void clearHandler();

	int64_t frame;
	//std::deque<Cmd> cmds;  //db deque
	/* data */
};


// #endif