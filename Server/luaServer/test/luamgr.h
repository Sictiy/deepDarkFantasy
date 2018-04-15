#ifndef __LUAMGR__
#define __LUAMGR__

#include "iostream"
extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

#define MAXLEN 1024*4
#include <string.h>

struct Msg{
	int fd;
	//int type;
	int cmd;
	char buff[MAXLEN];
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

	void disConnect(int fd);
	void recvData(Msg* msg);

	// void test();

private:
	lua_State* pLuaState;
	int pRef;
	/* data */
};

int luaLog(lua_State* L);
int luaSendData(lua_State* L);
int luaCreateServer(lua_State* L);
int luaConnectServer(lua_State* L);
std::string getString(lua_State* L,const char* name);
int getInt(lua_State* L, const char* name);

#define luaMgr LuaMgr::Instance()
#endif // __LUAMGR