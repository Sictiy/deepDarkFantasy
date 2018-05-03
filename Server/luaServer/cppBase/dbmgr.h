#pragma once
#include "head.h"
#include "dbthread.h"

extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

class DbMgr{
public:
	DbMgr();
	~DbMgr();

	bool init(lua_State* L);
	void clear();

	static DbMgr* Instance();
	DbThread* createThread(const char* script_name);
private:
	std::vector<DbThread*> threads;
	/* data */
};

int luaCreateThread(lua_State* L);
int luaMysqlConnect(lua_State* L);
int luaMysqlSelectDb(lua_State* L);
int luaMysqlQuery(lua_State* L);