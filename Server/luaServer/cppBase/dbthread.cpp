#include "dbthread.h"

IMPL_LUA_CLASS_BEGIN(DbThread)
	EXPORT_LUA_FUNCTION(luaPushRequest)
IMPL_LUA_CLASS_END()

DbThread::DbThread(){
	m_process = NULL;
	resQueue = new MsgQueue("res");
	reqQueue = new MsgQueue("req");
	m_mysql = NULL;
	runing = false;
}

DbThread::~DbThread(){
	clear();
}

bool DbThread::init(const char* script_name){
	if(!initMysql())
		return false;
	if(!initLua(script_name))
		return false;
	m_process = new std::thread(&DbThread::process,this);
	return true;
}

bool DbThread::initLua(const char* script_name){
	LuaMgr* luamgr = LuaMgr::Instance();
	lua_State* L = luamgr->getState();

	lua_pushlightuserdata(L,this);
	lua_pushcclosure(L,luaMysqlConnect,1);
	lua_setglobal(L,"mysqlConnect");

	lua_pushlightuserdata(L,this);
	lua_pushcclosure(L,luaMysqlQuery,1);
	lua_setglobal(L,"mysqlQuery");

	lua_pushlightuserdata(L,this);
	lua_pushcclosure(L,luaMysqlSelectDb,1);
	lua_setglobal(L,"mysqlSelectDb");

	if(!luamgr->loadScript(script_name)){
		std::cout<< "initlua false" <<std::endl;
		return false;
	}
	return true;
}

bool DbThread::initMysql(){
	my_bool optOn = true;

	m_mysql = mysql_init(NULL);
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &optOn);
	return true;
}

void DbThread::clear(){
	delete resQueue;
	delete reqQueue;
	if(m_mysql){
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
}

void DbThread::process(){
	runing = true;
	while(runing) {
		callStruct* request = NULL;
		callStruct* respond = NULL;
		int count = 0;
		while(true) {
			request = reqQueue->popMsg();
			if(request == NULL)
				break;
			processReq(request);
			free(request);
			count++;
		}
		while(true) {
			respond = resQueue->popMsg();
			if(respond == NULL)
				break;
			processRes(respond);
			free(respond);
			count++;
		}
		if(count == 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
}

void DbThread::processReq(callStruct* request){
	LuaMgr* luamgr = LuaMgr::Instance();
	lua_State* L = luamgr->getState();
	int top = lua_gettop(L);
	LuaPacker * packer = new LuaPacker();
	int paramCount = packer->unserilize(L,(const unsigned char*)request->data,request->dataLen);
	std::cout << "get request paramcount: "<< paramCount << std::endl;
	if(!(lua_isstring(L,top+1))){
		lua_settop(L,top);
		return ;
	}
	lua_pushvalue(L,top+1);
	lua_gettable(L,LUA_GLOBALSINDEX);
	if(!(lua_isfunction(L,-1))){
		lua_settop(L,top);
		return ;
	}
	lua_insert(L,top+2);
	if(!luamgr->luaCall(L,paramCount-1,LUA_MULTRET)){
		lua_settop(L,top);
		return ;
	}
	int resultCount = lua_gettop(L) - (top+1);
	packer->clear();
	if(!packer->pushValue(L,top+2,resultCount)){
		lua_settop(L,top);
		std::cout<<"pushValue failed!"<<std::endl;
		return ;
	}
	unsigned char * data = NULL;
	size_t dataLen = 0;
	packer->serialize(&data,&dataLen);

	callStruct* respond = NULL;
	respond = (callStruct*)malloc(sizeof(callStruct)+dataLen);
	respond->id = request->id;
	respond->dataLen = dataLen;
	if(dataLen > 0)
		memcpy(respond->data,data,dataLen);
	resQueue->pushMsg(respond);
}

void DbThread::processRes(callStruct* respond){
	LuaMgr* luamgr = LuaMgr::Instance();
	lua_State* L = luamgr->getState();
	int top = lua_gettop(L);
	LuaPacker * packer = new LuaPacker();
	Lua_GetObjFunction(L,this,"onResult");
	if(lua_isfunction(L,-1)){
		lua_pushinteger(L,respond->id);
		lua_pushlstring(L,(const char*)respond->data,respond->dataLen);
		luamgr->luaCall(L,2,0);
	}
	lua_settop(L,top);
}

int DbThread::luaPushRequest(lua_State* L){
	int requestId = (int)lua_tointeger(L,1);
	size_t dataLen = 0;
	const char* data = lua_tolstring(L,2,&dataLen);
	if(data == NULL)
		return 0;
	callStruct* request = NULL;
	request = (callStruct*)malloc(sizeof(callStruct)+dataLen);
	request->id = requestId;
	request->dataLen = dataLen;
	if(dataLen > 0)
		memcpy(request->data,data,dataLen);
	reqQueue->pushMsg(request);
	lua_pushboolean(L,true);
	return 1;
}

int DbThread::luaPushQueryResult(lua_State* L, MYSQL_RES* queryRet){
	int row = (int)mysql_num_rows(queryRet);
	int field = (int)mysql_num_fields(queryRet);
	std::cout << "quert result: row : "<<row << "field: "<< field <<std::endl; 
	MYSQL_FIELD* pfields = mysql_fetch_fields(queryRet);

	lua_newtable(L);
	lua_createtable(L, field, 0);
	for(int i=0; i< field; i++){
		lua_pushstring(L,pfields[i].name);
		lua_rawseti(L,-2,i+1);
	}
	lua_setfield(L,-2,"fields");

	lua_createtable(L,row,0);
	for(int i=0; i<row; i++){
		MYSQL_ROW prow = mysql_fetch_row(queryRet);
		unsigned long* lengths = mysql_fetch_lengths(queryRet);
		lua_newtable(L);
		// size_t fieldArray = luapack_newarray()
		for(int j = 0; j<field; j++){
			if(prow[j] != NULL){
				lua_pushlstring(L,prow[j],lengths[j]);
				lua_rawseti(L,-2,j+1);
			}
		}
		lua_rawseti(L,-2,i+1);
	}
	lua_setfield(L,-2,"rows");
	return 1;
}

MYSQL* DbThread::getMysql(){
	return m_mysql;
}
/*******************************/
int luaMysqlConnect(lua_State* L){
	DbThread* dbthread = (DbThread*)lua_touserdata(L,lua_upvalueindex(1));
	MYSQL* pmysql = dbthread->getMysql();
	const char* host = lua_tostring(L,1);
	int port = (int)lua_tointeger(L,2);
	const char* user = lua_tostring(L,3);
	const char* passwd = lua_tostring(L,4);
	MYSQL* pConnRet = NULL;

	pConnRet = mysql_real_connect(pmysql,host,user,passwd,"",port,NULL,CLIENT_FOUND_ROWS | CLIENT_MULTI_STATEMENTS);
	if(pConnRet == NULL){
		const char* errorInfo = mysql_error(pmysql);
		std::cout << errorInfo << std::endl;
		lua_pushstring(L,errorInfo);
		return  lua_error(L);
	}
	std::cout << "success connect db" << std::endl;
	return 0;
}
int luaMysqlSelectDb(lua_State* L){
	DbThread* dbthread = (DbThread*)lua_touserdata(L,lua_upvalueindex(1));
	MYSQL* pmysql = dbthread->getMysql();

	const char* dbName = lua_tostring(L,1);
	if( 0 != mysql_select_db(pmysql,dbName)){
		const char* errorInfo = mysql_error(pmysql);
		std::cout << errorInfo << std::endl;
		lua_pushstring(L,errorInfo);
		return lua_error(L);
	}
	std::cout << "success select db: " <<dbName<< std::endl;
	return 0;
}
int luaMysqlQuery(lua_State* L){
	DbThread* dbthread = (DbThread*)lua_touserdata(L,lua_upvalueindex(1));
	MYSQL* pmysql = dbthread->getMysql();

	size_t sqlLen = 0;
	const char* query = lua_tolstring(L,1,&sqlLen);
	while(true){
		if(mysql_real_query(pmysql,query,(unsigned long)sqlLen) == 0)
			break;
		unsigned int errorNo = mysql_errno(pmysql);
		if(errorNo != CR_SERVER_GONE_ERROR&& errorNo != CR_SERVER_LOST){
			const char* errorInfo = mysql_error(pmysql);
			std::cout << errorInfo << std::endl;
			lua_pushstring(L,errorInfo);
			return lua_error(L);
		}
		int pingCount = 0;
		unsigned long threadId = mysql_thread_id(pmysql);
		while(true) {
			pingCount++;
			int ret = mysql_ping(pmysql);
			if(ret == 0){
				unsigned long currentId = mysql_thread_id(pmysql);
				if(currentId != threadId){
					std::cout << "reconnected to mysql server" << std::endl;
					break;
				}else{
					std::cout << mysql_error(pmysql) << std::endl;
				}

				int msCount = 80;
				if(pingCount > 125){
					msCount = 1000;
				}else if(pingCount >135){
					msCount = 3000;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(msCount));
			}
		}
	}
	int result = 0;
	do{
		MYSQL_RES* queryRet = mysql_store_result(pmysql);
		if(queryRet){
			result += dbthread->luaPushQueryResult(L,queryRet);
			mysql_free_result(queryRet);
		}
	} while(0 == mysql_next_result(pmysql));
	std::cout << "query msyql success "<< query<< std::endl;
	return result;
}