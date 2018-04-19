#ifndef __MSGQUEUE__
#define __MSGQUEUE__

#include "head.h"

class MsgQueue{
public:
	MsgQueue(const char*);
	virtual ~MsgQueue();

	callStruct* popMsg(); 
	bool pushMsg(callStruct *callstruct);
	const char * getName(void);

private:
	std::deque<callStruct*> msgs;  //db deque
	char * msgName;
	std::mutex m_mutex;
	/* data */
};


#endif