#ifndef __DBHANDLER__
#define __DBHANDLER__

#include "handler.h"

class DbHandler:public Handler
{
public:
	DbHandler();
	virtual ~DbHandler();

  	virtual int handleMsg(Msg *msg);

};

#endif