#ifndef __MSGQUEUE__
#define __MSGQUEUE__

#include "head.h"

class MsgQueue{
public:
	MsgQueue(const char*);
	virtual ~MsgQueue();

	Msg* recvMsg(); 
	bool sendMsg(Msg *msg);
	const char * getName(void);

private:
	std::deque<Msg> msgs;  //db deque
	char * msgName;
	/* data */
};


#endif