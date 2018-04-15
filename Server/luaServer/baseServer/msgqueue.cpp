#include "msgqueue.h"

MsgQueue::MsgQueue(const char * pName){
	msgName =const_cast<char*>(pName);
}

MsgQueue::~MsgQueue(){
	delete 	msgName;
	msgs.clear();
}

Msg* MsgQueue::recvMsg(){
	if(msgs.empty()){
		return nullptr;
	}else{
		Msg *msg = &msgs.back();
		msgs.pop_back();
		//std::cout << "return a msg to mgr,length of queue:" <<msgs.size()<< std::endl;
		return msg;
	}
}

bool MsgQueue::sendMsg(Msg *msg){
	msgs.push_front(*msg);
	//std::cout << "add a msg to queue,length of queue:" <<msgs.size()<< std::endl;
}

const char * MsgQueue::getName(){
	return msgName;
}