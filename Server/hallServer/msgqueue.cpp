#include "msgqueue.h"

MsgQueue::MsgQueue(const char * pName){
	msgName =const_cast<char*>(pName);
}

MsgQueue::~MsgQueue(){
	delete 	msgName;
	cmds.clear();
}

Cmd* MsgQueue::recvMsg(){
	if(cmds.empty()){
		return nullptr;
	}else{
		Cmd *cmd = &cmds.back();
		cmds.pop_back();
		return cmd;
	}
}

bool MsgQueue::sendMsg(Cmd cmd){
	cmds.push_front(cmd);
}

const char * MsgQueue::getName(){
	return msgName;
}