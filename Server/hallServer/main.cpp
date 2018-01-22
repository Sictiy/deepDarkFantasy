#include "server.h"
#include "dbmgr.h"

int main(){
	Server server;
	server.createServer();
	server.run();
}