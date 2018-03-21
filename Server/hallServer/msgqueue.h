#ifndef __MSGQUEUE__
#define __MSGQUEUE__

#include "head.h"

class MsgQueue{
public:
	MsgQueue(const char*);
	virtual ~MsgQueue();

	Cmd* recvMsg(); 
	bool sendMsg(Cmd cmd);
	const char * getName(void);

private:
	std::deque<Cmd> cmds;  //db deque
	char * msgName;
	/* data */
};


#endif