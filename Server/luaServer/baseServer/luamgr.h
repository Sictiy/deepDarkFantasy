#ifndef __LUAMGR__
#define __LUAMGR__

#include "head.h"
#include "packet.h"
#include "iostream"
extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

class LuaMgr
{
public:
	LuaMgr();
	virtual ~LuaMgr();
	static LuaMgr &Instance(){
		static LuaMgr luaMgr;
		return luaMgr;
	};

	bool init();
	void clear();

	bool start();
	bool loadScript(const char * script_name, const char * lua_Name = NULL);
	bool luaCall(lua_State* L,int args,int results);

	void disConnect(Packet* packet);
	void recvData(Packet* packet);
	void newConnect(Packet* packet);

	// void test();

private:
	lua_State* pLuaState;
	// int pRef;
	/* data */
};
void pushLuaFunction(const char * name ,lua_State* L, Packet* packet, lua_CFunction func);
void pushPacket(lua_State* L, Packet* packet);

int luaLog(lua_State* L);
int luaSendData(lua_State* L);
int luaCreateServer(lua_State* L);
int luaConnectServer(lua_State* L);

std::string getString(lua_State* L,const char* name);
int getInt(lua_State* L, const char* name);

#define luaMgr LuaMgr::Instance()
#endif // __LUAMGR