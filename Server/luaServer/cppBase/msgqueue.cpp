#include "msgqueue.h"

MsgQueue::MsgQueue(const char * pName){
	msgName =const_cast<char*>(pName);
}

MsgQueue::~MsgQueue(){
	delete 	msgName;
	msgs.clear();
}

callStruct* MsgQueue::popMsg(){
	callStruct* callstruct = NULL;
	m_mutex.lock();
	if(!msgs.empty()){
		callstruct = msgs.back();
		msgs.pop_back();
	}
	m_mutex.unlock();
	return callstruct;
}

bool MsgQueue::pushMsg(callStruct* callstruct){
	m_mutex.lock();
	msgs.push_front(callstruct);
	m_mutex.unlock();
}

const char * MsgQueue::getName(){
	return msgName;
}