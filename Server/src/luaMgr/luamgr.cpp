#include "luamgr.h"

LuaMgr::LuaMgr(){
	pLuaState = NULL;
}

LuaMgr::~LuaMgr(){
	clear();
}

bool LuaMgr::init(){
	lua_State* L = NULL;
	L = luaL_newstate();
	// assert(L);
	luaL_openlibs(L);
	pLuaState = L;

	int retCode = loadScript("config.lua","Config");
	if(!retCode)
		std::cout << "can't find 'config.lua'"<< std::endl;
	return true;
}

void LuaMgr::clear(){
	if(pLuaState){
		lua_close(pLuaState);
		pLuaState = NULL;
	}
}

bool LuaMgr::loadScript(const char * script_name, const char * lua_Name){
	bool result = false;
	bool retCode = false;

	lua_State* L = pLuaState;
	int top = lua_gettop(L);

	luaL_loadfile(L,script_name);
	if(lua_isfunction(L,-1)){
		if(lua_Name != NULL){
			lua_getglobal(L,lua_Name);
			if(lua_isnil(L,-1)){
				lua_pop(L,1);
				lua_newtable(L);
				lua_newtable(L);

				lua_getglobal(L,"_G");
				lua_setfield(L,-2,"__index");

				lua_setmetatable(L,-2);

				lua_pushvalue(L,-1);
				lua_setglobal(L,lua_Name);
			}

			lua_setfenv(L,-2);
		}
		retCode = luaCall(L,0,0);
		if(retCode){
			result = true;
		}
	}
	lua_settop(L,top);
	return result;
}

bool LuaMgr::luaCall(lua_State * L, int args, int results){
	bool result = false;
	int retCode = 0;
	int funcIndex = lua_gettop(L)-args;
	if(funcIndex <= 0)
		return false;
	retCode = lua_pcall(L,args,results,0);
	if(retCode != 0)
		return false;
	return true;
}

bool LuaMgr::start(){
	return loadScript("main.lua",NULL);
}