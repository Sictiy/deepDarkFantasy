#ifndef __LUAMGR__
#define __LUAMGR__

#include "head.h"
#include "packet.h"
#include "iostream"
#include "server.h"
#include "luapacker.h"

extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

enum lua_fState{
	lua_p_load,
	lua_p_update,
	lua_p_shutdown
};

class LuaMgr
{
public:
	LuaMgr();
	virtual ~LuaMgr();
	static LuaMgr* Instance();

	bool init();
	void clear();

	bool start();
	void process(int64_t frame);
	bool isShutDown();
	bool loadScript(const char * script_name, const char * lua_Name = NULL);
	bool luaCall(lua_State* L,int args,int results);
	void setStateFunc(lua_State* L, lua_fState state, int index);
	void setErrorFunc(lua_State* L, int index);

	void disConnect(Packet* packet);
	void recvData(Packet* packet);
	void newConnect(Packet* packet);

	lua_State* getState();
	int getInt(const char *);
	std::string getString(const char *);

private:
	lua_State* pLuaState;
	lua_fState pState;
	bool pShutDown;
	int errorFunc;
	int stateFuncs[];
};
void pushLuaFunction(const char * name ,lua_State* L, Packet* packet, lua_CFunction func);
void pushPacket(lua_State* L, Packet* packet);

int luaLog(lua_State* L);
int luaSetStateFunc(lua_State* L);
int luaSetErrorFunc(lua_State* L);
int luaXCall(lua_State* L);
int luaSendData(lua_State* L);
int luaCreateServer(lua_State* L);
int luaConnectServer(lua_State* L);

int luaSerialize(lua_State* L);
int luaUnSerialize(lua_State* L);

std::string getString(lua_State* L,const char* name);
int getInt(lua_State* L, const char* name);

// #define luaMgr LuaMgr::Instance()
#endif // __LUAMGR