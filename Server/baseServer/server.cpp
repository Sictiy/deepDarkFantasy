#include "server.h"
#include "network.h"
#include "msgmgr.h"

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
	msgMgr->init();
	registerHandler();

	if(network->createServer("0.0.0.0",SERVERPORT)){
		//setLog();
		setDaemon();

		network->run();
		return true;
	}

	delete network;
	return false;
}

void Server::registerHandler(){
	msgMgr->addHandler(c2s_rank_get,new Handler());
}

void Server::clearHandler(){
	msgMgr->clearHandler(c2s_rank_get);
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