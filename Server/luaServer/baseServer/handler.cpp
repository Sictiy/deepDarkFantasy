#include "handler.h"

Handler::Handler(){

}

Handler::~Handler(){

}

void Handler::handleMsg(Msg *msg){
	std::cout << msg->fd << "--" << msg->cmd << std::endl;
}

void Handler::sendMsg(Msg *msg){
	Packet::sendMsg(msg->fd,msg);	
}

void Handler::sendData(int fd,int cmd, char * buff){
	Msg msg;
	msg.fd = fd;
	msg.cmd = cmd;
	memcpy(msg.buff,buff,strlen(buff));
	sendMsg(&msg);
}