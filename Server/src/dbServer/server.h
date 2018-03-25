#ifndef __SERVER__
#define __SERVER__

#include "../baseServer/head.h"

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
	void clearHandler();

	/* data */
};


#endif