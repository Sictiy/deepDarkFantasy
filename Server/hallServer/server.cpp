#include "server.h"
#include "client.h"
#include "network.h"

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

}

bool Server::init(){
	Network *network = new Network();
	Client *client = new Client();
	Dbmgr *dbmgr = new Dbmgr(); 

	dbmgr->setCmdDeque(cmds);
	client->setCmdDeque(cmds);

	network->setCliMgr(client);
	if(network->createServer("0.0.0.0",SERVERPORT)){
		//setLog();
		setDaemon();

		dbmgr->run();
		network->run();
		client->run();
		return true;
	}
	delete client;
	delete dbmgr;
	delete network;
	return false;
}

// void Server::setLog(){
// 	signal(SIGINT, sig_handler);
// 	signal(SIGTERM, sig_handler);
// 	signal(SIGQUIT, sig_handler);
// }

void Server::setDaemon(){
	if(-1==daemon(0,0)){
		std::cout << "daemon erro!" << std::endl;
		exit(1);
	}
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
}