#ifndef __LUAMGR__
#define __LUAMGR__

#include "iostream"
extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

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
	bool loadScript(const char * script_name, const char * lua_Name);
	bool luaCall(lua_State* L,int args,int results);

private:
	lua_State* pLuaState;

	/* data */
};

#define luaMgr LuaMgr::Instance()

#endif // __LUAMGR
