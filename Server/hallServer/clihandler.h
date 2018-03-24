#ifndef __CLIHANDLER__
#define __CLIHANDLER__

#include "handler.h"

class CliHandler:public Handler
{
public:
	CliHandler();
	virtual ~CliHandler();

  	virtual int handleMsg(Msg *msg);

};

#endif