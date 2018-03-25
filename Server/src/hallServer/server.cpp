#include "server.h"
#include "clihandler.h"
#include "dbhandler.h"
#include "../baseServer/network.h"
#include "../baseServer/msgmgr.h"

/*******************************************/
bool loop = true;
int DbHandler::dbFd = 0;
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

	if(network->createServer("0.0.0.0",HALLPORT)){
		setDaemon();

		msgMgr.init();
		registerHandler();
		int dbFd =  network->connectServer("127.0.0.1",DBPORT);
		std::cout << "connectDbServer,fd:" << dbFd << std::endl;

		// DbHandler::dbFd = dbFd;
		DbHandler::selectRank(dbFd);

		network->run();
		return true;
	}

	delete network;
	return false;
}

void Server::registerHandler(){
	msgMgr.addHandler(fd_newconnect,new CliHandler());
	msgMgr.addHandler(c2s_rank_get,new CliHandler());
	msgMgr.addHandler(d2h_get,new DbHandler());
	msgMgr.addHandler(d2h_insert,new DbHandler());
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