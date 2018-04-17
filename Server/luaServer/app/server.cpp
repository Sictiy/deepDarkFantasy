#include "server.h"
#include "../baseServer/luamgr.h"
#include "../baseServer/network.h"
// #include "../baseServer/msgmgr.h"

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
	frame = 0;
}

Server::~Server(){
	clearHandler();
}

bool Server::init(){
	// setDaemon();

	luaMgr.init();
	network.init();
	return true;
}

void Server::run(){
	using namespace std::chrono;
    steady_clock::time_point tpBegin = steady_clock::now();

	luaMgr.start();
	network.run();
	int fps = luaMgr.getInt("FPS");
	std::cout << "fps: "<< fps << std::endl;
	while(true){
        steady_clock::time_point tpNow = steady_clock::now();
        milliseconds dur = duration_cast<milliseconds>(tpNow - tpBegin);
		if(dur.count()* fps < frame * 1000){
			std::this_thread::sleep_for(milliseconds(8));
			continue;
		}        
		frame++;
		network.breathe();
		if(luaMgr.isShutDown)
			break;
	}
}

bool Server::isQuit(){
	return loop;
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