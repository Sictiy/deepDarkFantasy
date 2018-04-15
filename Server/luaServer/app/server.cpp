#include "server.h"
#include "../baseServer/luamgr.h"
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
	setDaemon();

	msgMgr.init();
	luaMgr.init();

	return true;
}

void Server::run(){
	using namespace std::chrono;
	msgMgr.run();
	luaMgr.start();
	network.run();
	while(loop){
		std::this_thread::sleep_for(milliseconds(2000));
	}
}

void Server::registerHandler(){
}

void Server::clearHandler(){
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