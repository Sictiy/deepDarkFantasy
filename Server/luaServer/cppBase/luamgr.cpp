#include "luamgr.h"
// #include "../baseServer/packet.h"
// #include "../baseServer/msgmgr.h"
// #include "../baseServer/network.h"
// #include "msgmgr.h"
#include "network.h"
#include "server.h"

LuaMgr* luamgr = NULL;

LuaMgr::LuaMgr(){
	pLuaState = NULL;
	pShutDown = false;
	pState = lua_p_load;
	stateFuncs[lua_p_load] = LUA_NOREF;
	stateFuncs[lua_p_update] = LUA_NOREF;
	stateFuncs[lua_p_shutdown] = LUA_NOREF;
	errorFunc = LUA_NOREF;
}

LuaMgr* LuaMgr::Instance(){
	if(luamgr == NULL){
		luamgr = new LuaMgr();
	}
	return luamgr;
}

LuaMgr::~LuaMgr(){
	clear();
}

void LuaMgr::clear(){
	if(pLuaState){
		// for(int nRef: stateFuncs){
		// 	if(nRef != LUA_NOREF){
		// 		lua_unref(pLuaState,nRef);
		// 	}
		// }

		lua_close(pLuaState);
		pLuaState = NULL;
	}
}

bool LuaMgr::init(){
	lua_State* L = NULL;
	L = luaL_newstate();
	// assert(L);
	luaL_openlibs(L);
	pLuaState = L;


	/***************************/
	lua_pushcfunction(L, luaLog);
    lua_setglobal(L, "Log");

    lua_pushcfunction(L,luaConnectServer);
    lua_setglobal(L, "ConnectServer");

    lua_pushcfunction(L,luaCreateServer);
    lua_setglobal(L, "CreateServer");

    lua_pushlightuserdata(L,this);
    lua_pushcclosure(L,luaSetStateFunc,1);
    lua_setglobal(L,"SetStateFunc");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, luaSetErrorFunc, 1);
    lua_setglobal(L, "SetErrorFunc");

    lua_pushlightuserdata(L,this);
    lua_pushcclosure(L, luaXCall,1);
    lua_setglobal(L,"Call");

    lua_pushcfunction(L,luaSerialize);
    lua_setglobal(L,"Serialize");

    lua_pushcfunction(L,luaUnSerialize);
    lua_setglobal(L,"UnSerialiaze");

	bool retCode = loadScript("config.lua","Config");
	if(!retCode)
		std::cout << "can't find 'config.lua'"<< std::endl;
	return true;
}

bool LuaMgr::start(){
	bool result = loadScript("main.lua");
	if(stateFuncs[pState] == LUA_NOREF)
		pState = lua_p_update;
	return result;
}

bool LuaMgr::isShutDown(){
	if(pShutDown)
		std::cout << "lua shutdown" << std::endl;
	return pShutDown;
}

void LuaMgr::process(int64_t frame){
	bool ret = false;
	int nRef = stateFuncs[pState];
	if(nRef != LUA_NOREF){
		lua_State* L = pLuaState;
		int top = lua_gettop(L);

		lua_getref(L,nRef);
		lua_pushinteger(L,(lua_Integer)frame);
		luaCall(L,1,1);
		ret = lua_toboolean(L,-1);
		lua_settop(L,top);
	}
	Server* pserver = Server::Instance();
	switch(pState){
		case lua_p_load:
			if(ret)
				pState = lua_p_update;
			else
				std::cout << "load failed !" << std::endl;
			break;
		case lua_p_update:
			if(pserver->isQuit()){
				if(stateFuncs[lua_p_shutdown] != LUA_NOREF)
					pState = lua_p_shutdown;
				else
					pShutDown = true;
			}
			break;
		case lua_p_shutdown:
			if(ret)
				pShutDown = true;
			std::cout<< "process shutdown" << std::endl;
			break;
	}
}

bool LuaMgr::loadScript(const char * script_name, const char * lua_Name){
	bool result = false;
	bool retCode = false;

	lua_State* L = pLuaState;
	int top = lua_gettop(L);

	int loadRet = luaL_loadfile(L,script_name);
	if(loadRet){
		std::cout << "load failed!" << std::endl;
		lua_settop(L,top);
		return result;
	}
	std::cout << "loadScript "<<script_name << std::endl;
	if(lua_isfunction(L,-1)){
		std::cout << "lua_isfunction" << std::endl;
		if(lua_Name != NULL){
			lua_getglobal(L,lua_Name);
			std::cout << "lua_name is nill" << std::endl;
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
	int errorIndex = 0;
	// if(errorFunc!= LUA_NOREF){
	// 	lua_getref(L,errorFunc);
	// 	errorIndex = -1;
	// }

	retCode = lua_pcall(L,args,results,errorIndex);
	if(retCode != 0){
		std::cout << "lua_pcall" << retCode << args << results << 0 << std::endl;
		const char* errorInfo = lua_tostring(L,-1);
		if(errorInfo&&errorInfo[0])
			std::cout <<"errorInfo: "<< errorInfo << std::endl;
		return false;
	}
	return true;
}

void LuaMgr::setStateFunc(lua_State* L, lua_fState state, int index){
	int nRef = stateFuncs[state];
	if(nRef != LUA_NOREF){
		lua_unref(L,nRef);
		stateFuncs[state] = LUA_NOREF;
	}
	if(lua_isfunction(L,index)){
		lua_pushvalue(L,index);
		stateFuncs[state] = lua_ref(L,true);
	}
}

void LuaMgr::setErrorFunc(lua_State* L,int index){
	int nRef = errorFunc;
	if(nRef != LUA_NOREF){
		lua_unref(L,nRef);
		errorFunc = LUA_NOREF;
	}
	if(lua_isfunction(L,index)){
		lua_pushvalue(L,index);
		errorFunc = lua_ref(L,true);
		std::cout << "set errorFunc success "<< std::endl;
	}
}
/*********************************************function to network*/
void LuaMgr::recvData(Packet * packet){
	std::cout << "recvData from: " << packet->getFd() << std::endl; 
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	int nRef =  packet->getRef();
	lua_getref(L,nRef);
	lua_getfield(L,-1,"recvData");
	if(lua_isfunction(L,-1)){
		// lua_pushnumber(L,packet->getFd());
		// lua_pushnumber(L,packet->getCmd());
		// lua_pushlstring(L,packet->getBuff(),strlen(packet->getBuff()));
		pushPacket(L,packet);
		luaCall(L,1,0);
	}else{
		std::cout << "can't find function" << std::endl;
	}
	lua_settop(L,top);
}

void LuaMgr::disConnect(Packet * packet){
	std::cout << "disconnect from: " << packet->getFd() << std::endl; 
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	int nRef = packet->getRef();
	lua_getref(L,nRef);
	lua_getfield(L,-1,"disConnect");
	if(lua_isfunction(L,-1)){
		// lua_pushnumber(L,packet->getFd());
		pushPacket(L,packet);
		luaCall(L,1,0);
	}else{
		std::cout << "can't find function" << std::endl;
	}
	lua_settop(L,top);
}

void LuaMgr::newConnect(Packet* packet){
	std::cout << "newconnect from: " << packet->getFd() << std::endl; 
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	Network* network = Network::Instance();
	Packet* listen = network->getListen();
	int nRef = listen->getRef();
	lua_getref(L,nRef);
	lua_getfield(L,-1,"newConnect");
	if(lua_isfunction(L,-1)){
		// lua_pushnumber(L,packet->getFd());
		pushPacket(L,packet);
		luaCall(L,1,0);
	}else{
		std::cout << "can't find function" << std::endl;
	}
	lua_settop(L,top); 
}

int LuaMgr::getInt(const char* name){
	lua_State* L = pLuaState;
	int top	= lua_gettop(L);
	lua_getglobal(L,name);
	int result = lua_tonumber(L,-1);
	lua_settop(L,top);
	return result;	
}

std::string LuaMgr::getString(const char* name){
	lua_State* L = pLuaState;
	int top	= lua_gettop(L);
	lua_getglobal(L,name);
	std::string string = lua_tostring(L,-1);
	lua_settop(L,top);
	return string;
}

lua_State* LuaMgr::getState(){
	return pLuaState;
}
/*******************************************function to lua*/
int luaLog(lua_State* L){
	bool retCode = false;
	const char* log_text = lua_tostring(L,1);
	if(log_text){
		std::cout << log_text << std::endl;
	}else{
		std::cout << "read log_text failed!" << std::endl;
	}
	return 0;
}

int luaSetStateFunc(lua_State* L){
	LuaMgr* luamgr = (LuaMgr*)lua_touserdata(L, lua_upvalueindex(1));
	lua_getfield(L,1,"load");
	luamgr->setStateFunc(L,lua_p_load,-1);

	lua_getfield(L,1,"update");
	luamgr->setStateFunc(L,lua_p_update,-1);

	lua_getfield(L,1,"shutdown");
	luamgr->setStateFunc(L,lua_p_shutdown,-1);
	return 0;
}

int luaSetErrorFunc(lua_State* L){
	LuaMgr* luamgr = (LuaMgr*)lua_touserdata(L, lua_upvalueindex(1));
	luamgr->setErrorFunc(L,1);
	return 0;
}

int luaXCall(lua_State* L){
	int nRet = 0;
	LuaMgr* luaMgr = (LuaMgr*)lua_touserdata(L,lua_upvalueindex(1));
	int top = lua_gettop(L);
	// std::cout<< "top: "<<top << std::endl;
	if(luaMgr->luaCall(L,top-1,LUA_MULTRET)){
		nRet = lua_gettop(L);
	}
	return nRet;
}

int luaConnectServer(lua_State* L){
	std::cout << "start connect server"<< std::endl;
	int top = lua_gettop(L);
	if (top < 0)
		return 0;
	const char* sIp = lua_tostring(L,1);
	int nPort = lua_tointeger(L,2);
	Network* network = Network::Instance();
	int fd = 0;
	fd = network->connectServer(sIp,nPort);
	if(fd > 0){
		std::cout << "connectServer success : [" << sIp<<":"<<nPort<<"] socket: "<< fd<<std::endl;
	}
	Packet* packet = network->getPacket(fd);
	pushPacket(L,packet);
	return 1;
}

int luaCreateServer(lua_State* L){
	int top = lua_gettop(L);
	if (top < 0)
		return 0;
	const char* sIp = lua_tostring(L,1);
	int nPort = lua_tointeger(L,2);
	Network* network = Network::Instance();
	if(network->createServer(sIp,nPort)){
		std::cout << "createServer success : [" << sIp<<":"<<nPort<<"] "<<std::endl;
	}
	Packet* packet = network->getListen();
	pushPacket(L,packet);
	return 1;
}

int luaSendData(lua_State* L){
	int top = lua_gettop(L);
	if (top < 0)
		return 0;
	Packet* packet = (Packet*)lua_touserdata(L,lua_upvalueindex(1));
	short cmd = (short)lua_tonumber(L,1);
	size_t length = 0;
	const char* data =lua_tolstring(L,2,&length);
	packet->sendData(length,cmd,data);
	return 0;
}

int luaSerialize(lua_State* L){
	int top = lua_gettop(L);
	if(top <= 0 )
		return 0;
	LuaPacker * packer = new LuaPacker();
	packer-> pushValue(L,1,top);
	size_t dataLen = 0;
	unsigned char* data = NULL;
	packer-> serialize(&data, &dataLen);
	uint32_t value = 0;
	memcpy(&value,data+6,4);
	std::cout << "serialize success : "<< value<<" length : "<<dataLen<< std::endl;
	lua_pushlstring(L, (const char*)data, dataLen);
	delete packer;
	return 1;
}

int luaUnSerialize(lua_State* L){
	if(!lua_isstring(L,1))
		return 0;
	size_t dataLen = 0;
	const char* data = lua_tolstring(L,1,&dataLen);
	uint32_t value = 0;
	memcpy(&value,data+6,4);
	std::cout << "get data success : "<< value<<" length : "<<dataLen<< std::endl;
	LuaPacker * packer = new LuaPacker();
	int results = packer->unserilize(L,	(const unsigned char*)data, dataLen);
	std::cout<< "results counts:"<<results <<std::endl;
	delete packer;
	return results;
}

/*******************************************function to get data from lua*/
void pushLuaFunction(const char * name ,lua_State* L, Packet* packet, lua_CFunction func){
	lua_pushstring(L,name);
	lua_pushlightuserdata(L,packet);
	lua_pushcclosure(L,func,1);
	lua_rawset(L,-3);
	std::cout << "pushluaFunction success: "  << std::endl;
}

void pushPacket(lua_State* L, Packet* packet){
	int nRef = 0;
	if(!packet){
		lua_pushnil(L);
		std::cout << "pushPacket failed: " <<nRef << std::endl;
		return;
	}
	nRef = (int)(intptr_t)packet->getRef();
	if(nRef != LUA_NOREF){
		lua_getref(L,nRef);
		std::cout << "pushPacket getRef: " <<nRef << std::endl;
		return;
	}

	// lua_newtable(L);

	// lua_pushstring(L,"__packet");
	// lua_pushlightuserdata(L,packet);
	Lua_PushObject(L,packet);
	// lua_rawset(L,-3);

	// pushLuaFunction("SendData",L,packet,luaSendData);

	lua_pushvalue(L,-1);
	nRef = lua_ref(L,true);
	packet->setRef(nRef);
	std::cout << "pushPacket success: " <<nRef <<  "top: "<<lua_gettop(L)<< std::endl;
}