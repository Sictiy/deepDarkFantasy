#include "packet.h"

Packet::Packet(int _fd){
	fd = _fd;
	offset = 0;
	length = MAXLEN;
	type = 0;
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
		length = cache[0]+(cache[1]<<8);
	}
	
	if(offset >= length){ //get all request data
		short cmd =cache[2]+(cache[3]<<8);
		std::cout << "cmd:" << cmd << std::endl;
		cmd = c2s_rank_get;
		memcpy(buff,cache+4,length-4);
		std::cout <<"bufflen:"<< strlen(buff)<<std::endl;
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

short Packet::getType(){
	return type;
}

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