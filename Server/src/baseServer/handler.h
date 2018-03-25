#ifndef __HANDLER__
#define __HANDLER__

#include "head.h"
#include "packet.h"

class Handler
{
public:
	Handler();
	virtual ~Handler();

  	virtual void handleMsg(Msg *msg);
  	static void sendData(int fd,int cmd, char* buff);
	static void sendMsg(Msg *msg);
protected:
	
};
#endif
