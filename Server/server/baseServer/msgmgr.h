#ifndef __MSGMGR__
#define __MSGMGR__

#include "head.h"
#include "handler.h"
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
	void addHandler(int,Handler*);
	void removeHandler(int, Handler*);
	void clearHandler(int);
	void dispatchMsg(Msg *msg);
	void dispatchMsgToLua(Msg* msg);
	void addMsg(Msg *msg);
	void run();

private:
	void process();
	std::thread* Thread;
	std::map<int, HandlerList> handler_map_;
	MsgQueue * msgs;
};

#define msgMgr MsgMgr::Instance()

#endif