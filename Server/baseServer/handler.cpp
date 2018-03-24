#include "handler.h"

Handler::Handler(){

}

Handler::~Handler(){

}

void Handler::handlerMsg(Msg *msg){
	std::cout << msg->fd << "--" << msg->cmd << endl;
}