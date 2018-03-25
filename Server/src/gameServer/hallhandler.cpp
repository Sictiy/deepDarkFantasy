#include "hallhandler.h"

HallHandler::HallHandler(){

}

HallHandler::~HallHandler(){

}

void HallHandler::handleMsg(Msg *msg){
	std::cout << msg->fd << "--" << msg->cmd << std::endl;
}