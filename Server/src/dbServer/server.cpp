#include "server.h"
#include "hallhandler.h"
#include "dbmgr.h"
#include "../baseServer/network.h"
#include "../baseServer/msgmgr.h"

/*******************************************/
bool loop = true;
void sig_handler( int sig )
{
    if ( sig == SIGINT ||sig == SIGQUIT||sig == SIGTERM)
    {
        loop = false;
    }
}
/*******************************************/
Server::Server(){

}

Server::~Server(){
	clearHandler();
}

bool Server::init(){
	Network *network = new Network();

	if(network->createServer("0.0.0.0",DBPORT)){
		//setLog();
		setDaemon();

		msgMgr.init();
		dbMgr.init();
		registerHandler();
		network->run();
		return true;
	}

	delete network;
	return false;
}

void Server::registerHandler(){
	msgMgr.addHandler(h2d_get,new HallHandler());
	msgMgr.addHandler(h2d_insert,new HallHandler());
	std::cout << "handlers is registed!" << std::endl;
}

void Server::clearHandler(){
	msgMgr.clearHandler(c2s_rank_get);
}

void Server::setDaemon(){
	if(-1==daemon(0,0)){
		std::cout << "daemon erro!" << std::endl;
		exit(1);
	}
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
}