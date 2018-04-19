#include "dbmgr.h"

DbMgr* dbmgr = NULL;
DbMgr::DbMgr(){

}

DbMgr::~DbMgr(){
	clear();
}

bool DbMgr::init(lua_State* L){
	lua_pushcfunction(L,luaCreateThread);
	lua_setglobal(L,"CreateThread");
	lua_pushcfunction(L,)
}

void DbMgr::clear(){
	for(auto dbthrea : threads){
		delete dbthread;
	}
	threads.clear();
}

DbThread* DbMgr::createThread(const char* script_name){
	DbThread* newThread = new DbThread();
	if(!newThread->init(script_name)){
		delete newThread;
		return NULL;
	}
	threads.push_back(newThread);
	return newThread;
}

DbMgr* DbMgr::Instance(){
	if(dbmgr == NULL)
		dbmgr = new DbMgr();
	return dbmgr;
}
/****************************************************/
int luaCreateThread(lua_State* L){
	DbMgr* dbmgr = DbMgr::Instance();
	const char* script_name = lua_tostring(L,1);
	if(script_name == NULL)
		return 0;
	DbThread* newThread= dbmgr->createThread(script_name);
	return newThread;
}