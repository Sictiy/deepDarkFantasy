#include "packet.h"

Packet::Packet(int _fd){
	fd = _fd;
	offset = 0;
	length = MAXLEN;
	//type = 0;
}

Packet::~Packet(){

}

int Packet::addPacket(int fd){
	int readlen = read(fd, cache+offset, MAXLEN-offset);
	offset += readlen;
	if(offset <= 0){
		return -1;
	}

	if(offset >= 2){
		// length = cache[0]+(cache[1]<<8);
		memcpy(&length,cache,2);
		std::cout << "bufflen :" << length <<std::endl;
	}
	
	if(offset >= length){ //get all request data
		cmd =cache[2]+(cache[3]<<8);
		std::cout << "cmd:" << cmd << std::endl;
		memcpy(buff,cache+4,length-4);
		// std::cout <<"bufflen:"<< strlen(buff)<<std::endl;
		//add packet to client

		offset -= length;
		if(offset > 0){
			memcpy(cache,cache+length,offset);
		}
		length = MAXLEN;
		return 1;
	}
	return 0;
}

Msg Packet::recvMsg(){
	Msg msg;
	msg.fd = getFd();
	msg.cmd = getCmd();
	memcpy(msg.buff,getBuff(),strlen(getBuff()));
	return msg;
}

void Packet::sendMsg(int fd,const Msg* msg){
	short len = strlen(msg->buff)+4;
	char * datatoh = new char[len];

    bzero(datatoh,strlen(datatoh));
	memcpy(datatoh,&len,2);
	memcpy(datatoh+2,&(msg->cmd),2);
	memcpy(datatoh+4,msg->buff,len-4);

	std::cout << "send length:" << len<< " fd:"<<fd<<std::endl;
	int sendlen = 0;
	while(sendlen < len){
		sendlen += write(fd, datatoh+sendlen, len-sendlen);
	}
}

// short Packet::getType(){
// 	return type;
// }

short Packet::getLength(){
	return length;
}

char* Packet::getBuff(){
	return buff;
}

int Packet::getFd(){
	return fd;
}

int Packet::getCmd(){
	return cmd;
}
