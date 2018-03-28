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
		memcpy(&length,cache,2);
		std::cout << "length:" << length << "++" << cache[0]+(cache[1])<< std::endl;
		// length = cache[0]+(cache[1]<<8);
	}
	
	if(offset >= length){ //get all request data
		cmd =cache[2]+(cache[3]<<8);
		std::cout << "cmd:" << cmd << std::endl;
		memcpy(buff,cache+4,length-4);
		std::cout <<"bufflen:"<< strlen(buff)<<std::endl;
		// std::cout <<"buff:"<< buff<<std::endl;

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
	bzero(getBuff(),strlen(getBuff()));
	return msg;
}

void Packet::sendMsg(int fd,const Msg* msg){
	short length = strlen(msg->buff)+4;
	std::cout << "send length:" << length<< "	fd:"<<fd<<std::endl;
	char * datatoh = new char[length];
    bzero(datatoh,strlen(datatoh));
	memcpy(datatoh,&length,2);
	memcpy(datatoh+2,&(msg->cmd),2);
	memcpy(datatoh+4,msg->buff,length-4);
	int sendlen = 0;
	while(sendlen < length){
		sendlen += write(fd, datatoh+sendlen, length-sendlen);
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
