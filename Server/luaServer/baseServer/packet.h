#ifndef __PACKET__
#define __PACKET__

#include "head.h"

// IMPL_LUA_CLASS_BEGIN(Packet)
// 	EXPORT_LUA_FUNCTION(sendData)
// IMPL_LUA_CLASS_END()
extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

enum PacketType {
	_Server,
	_Client
};

class Packet
{
public:
	Packet(int);
	virtual ~Packet();
	// DECLARE_LUA_CLASS(Packet);

	int addPacket(int fd);
	short getLength();
	char* getBuff();
	int getFd();
	short getCmd();
	void setType(PacketType);
	bool needClose(int fps);
	void breathe();
	void breatheGet();
	void setRef(int nRef);
	int getRef();
	// static void sendMsg(int fd,const Msg *msg);
	void sendData(short length, short cmd,const char * data);
	// Msg recvMsg();

private:
	int fd;
	short cmd;
	PacketType type;
	char buff[MAXLEN];

	char cache[MAXLEN];
	short length;
	short offset;

	short breathe_count;

	int nRef;
	/* data */
};

#endif