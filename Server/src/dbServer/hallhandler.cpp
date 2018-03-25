#include "hallhandler.h"
#include "dbmgr.h"

HallHandler::HallHandler(){

}

HallHandler::~HallHandler(){

}

void HallHandler::handleMsg(Msg *msg){
	std::cout << msg->fd << "--" << msg->cmd << std::endl;
	switch(msg->cmd){
		case h2d_get:
			get(msg);
			break;
		case h2d_insert:
			insert(msg);
			break;
		default:
			break;
	}
}

void HallHandler::get(Msg *msg){
	dbMgr.select(msg->fd);
}

void HallHandler::insert(Msg *msg){
	dbMgr.insert(msg);
}