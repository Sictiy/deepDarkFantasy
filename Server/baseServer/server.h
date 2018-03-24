#ifndef __SERVER__
#define __SERVER__

#include "head.h"

class Server{
public:
	Server();
	virtual ~Server();

	bool init();
	//bool createServer(const char* ip, int port);

private:
	//void setLog();
	void setDaemon();
	void registerHandler();

	std::deque<Cmd> cmds;  //db deque
	/* data */
};


#endif