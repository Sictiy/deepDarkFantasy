#ifndef __SERVER__
#define __SERVER__

#include "../baseServer/head.h"

class Server{
public:
	Server();
	static Server &Instance(){
		static Server _inst;
		return _inst;
	}
	virtual ~Server();

	bool init();
	//bool createServer(const char* ip, int port);

private:
	//void setLog();
	void setDaemon();
	void registerHandler();
	void clearHandler();
	//bool connectDbServer(const char* ip, int port);

	//std::deque<Cmd> cmds;  //db deque
	/* data */
};

#define server Server::Instance()

#endif