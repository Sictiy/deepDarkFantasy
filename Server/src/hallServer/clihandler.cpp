#include "clihandler.h"
#include "client.h"

CliHandler::CliHandler(){

}

CliHandler::~CliHandler(){

}

void CliHandler::handleMsg(Msg *msg){
	std::cout << msg->fd << "--" << msg->cmd << std::endl;
	switch(msg->cmd){
		case fd_newconnect:
			Iclient.getRank(msg);
			break;
		case c2s_rank_get:
			Iclient.insert(msg);
			break;
		default:
			break;
	}
}