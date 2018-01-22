#include "client.h"

Client::Client(fd):
Fd(fd){

}

Client::~Client(){

}

Client::disconnected(){
	close(Fd);
}

CLient::process(){
	sendData(Buff);
}

CLient::sendData(char * data){
	std::string str("request:");
	str.append(data);
	write(Fd, str.c_str(), str.length());
}

Client::getBuff(){
	return Buff;
}