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
		return msg;
	}
}

bool MsgQueue::sendMsg(Msg msg){
	msgs.push_front(msg);
}

const char * MsgQueue::getName(){
	return msgName;
}