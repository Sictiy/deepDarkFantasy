#pragma once
#include "msgqueue.h"
#include "head.h"
#include "luamgr.h"

extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

class DbThread
{
public:
	DbThread();
	~DbThread();
	DECLARE_LUA_CLASS(DbThread);

	bool init(const char* script_name);
	bool initLua(const char* script_name);
	bool initMysql();
	void clear();
	MYSQL* getMysql();

	int luaPushRequest(lua_State* L);
	int luaPushQueryResult(lua_State* L, MYSQL_RES* queryRet);
private:
	void process();
	void processRes(callStruct*);
	void processReq(callStruct*);
	bool runing;
	MsgQueue* reqQueue;
	MsgQueue* resQueue;
	std::thread* m_process;
	MYSQL* m_mysql; 
	/* data */
};

int luaMysqlConnect(lua_State* L);
int luaMysqlSelectDb(lua_State* L);
int luaMysqlQuery(lua_State* L);