#ifndef __MSGMGR__
#define __MSGMGR__

#include "head.h"
#include "handler.h"
#include "msg.h"
#include "msgqueue.h"

typedef std::list<Handler *> HandlerList;
typedef std::map<int, HandlerList> HandlerMap;

class MsgMgr
{
public:
	MsgMgr();
	static MsgMgr &Instance(){
    static MsgMgr inst_;
    return inst_;
   	}
	virtual ~MsgMgr();

	void init();
	void addHandler();
	void removeHandler();
	void clearHandler();
	void dispatchMsg(Msg *msg);
	void addMsg(Msg *msg);
private:

	void run();
	void process();
	std::map<int, HandlerList> HandlerMap;
	MsgQueue * msgs;
};

#endif