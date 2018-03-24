#ifndef __HANDLER__
#define __HANDLER__

#include "head.h" 

class Handler
{
public:
	Handler();
	virtual ~Handler();

  	void handleMsg(Msg *msg);
private:
	
};
#endif
