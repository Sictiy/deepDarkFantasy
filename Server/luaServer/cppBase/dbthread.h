#pragma once
#include "msgqueue.h"
#include "head.h"
#include "luamgr.h"

class DbThread
{
public:
	DbThread(arguments);
	~DbThread();
	DECLARE_LUA_CLASS(DbThread);

	bool init(const char* script_name);
	bool initLua(const char* script_name);
	void clear();
	MSYQL* getMysql();

	int luaPushRequest(lua_State* L);
private:
	void process();
	int processRes(callStruct*);
	int processReq(callStruct*);
	bool runing;
	MsgQueue* reqQueue;
	MsgQueue* resQueue;
	std::thread m_process;
	MYSQL* m_mysql; 
	/* data */
};

int luaMysqlConnect(lua_State* L);
int luaMysqlSelectDb(lua_State* L);
int luaMysqlQuery(lua_State* L);