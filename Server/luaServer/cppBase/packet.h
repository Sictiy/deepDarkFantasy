#ifndef __PACKET__
#define __PACKET__

#include "head.h"

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
	DECLARE_LUA_CLASS(Packet);

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

	int luaGetCmd(lua_State* L);
	int luaRecvData(lua_State* L);
	int luaSendData(lua_State* L);
	// Msg recvMsg();

private:
	int fd;
	short cmd;
	PacketType type;
	char buff[MAXLEN];
	short bufflen;

	char cache[MAXLEN];
	short length;
	short offset;

	short breathe_count;

	int nRef;
	/* data */
};


#endif