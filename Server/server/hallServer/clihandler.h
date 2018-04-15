#ifndef __CLIHANDLER__
#define __CLIHANDLER__

#include "../baseServer/handler.h"

class CliHandler:public Handler
{
public:
	CliHandler();
	virtual ~CliHandler();

  	virtual void handleMsg(Msg *msg);
private:

};

#endif