#include "client.h"

Client::Client(int fd):
Fd(fd)
{

}

Client::~Client(){

}

void Client::disconnected(){
	close(Fd);
}

void Client::process(){
	std::cout<<"BUFF:"<<Buff << std::endl;
	sendData(Buff);
}

void Client::sendData(char * data){
	std::string str("request:");
	str.append(data);
	write(Fd, str.c_str(), str.length());
}

char* Client::getBuff(){
	return Buff;
}