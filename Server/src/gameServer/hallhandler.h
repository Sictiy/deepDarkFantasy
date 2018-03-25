#ifndef __HALLHANDLER__
#define __HALLHANDLER__

#include "../baseServer/handler.h"

class HallHandler:public Handler
{
public:
	HallHandler();
	~HallHandler();

	virtual void handleMsg(Msg *msg);
	
};

#endif