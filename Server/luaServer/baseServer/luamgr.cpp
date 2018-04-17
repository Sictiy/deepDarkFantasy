#include "luamgr.h"
// #include "../baseServer/packet.h"
// #include "../baseServer/msgmgr.h"
// #include "../baseServer/network.h"
// #include "msgmgr.h"
#include "network.h"

LuaMgr::LuaMgr(){
	pLuaState = NULL;
	pState = LUA_NOREF;
	stateFuncs[lua_load] = LUA_NOREF;
	stateFuncs[lua_update] = LUA_NOREF;
	stateFuncs[lua_shutdown] = LUA_NOREF;
	pShutDown = false;
}

LuaMgr::~LuaMgr(){
	clear();
}

void LuaMgr::clear(){
	if(pLuaState){
		if(auto nRef: stateFuncs){
			if(nRef != LUA_NOREF){
				lua_unref(pLuaState,nRef);
			}
		}

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

	bool retCode = loadScript("config.lua","Config");
	if(!retCode)
		std::cout << "can't find 'config.lua'"<< std::endl;
	return true;
}

bool LuaMgr::start(){
	return loadScript("main.lua");
}

bool LuaMgr::isShutDown(){
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
		luaCall(L,1,0);
		ret = lua_toboolean(L,-1);
		lua_settop(L,top);
	}

	switch(pState){
		case lua_load:
			if(ret)
				pState = lua_update;
			break;
		case lua_update:
			if(server.isQuit()){
				if(stateFuncs[lua_shutdown] != lua_shutdown)
					pState = lua_shutdown;
				else
					pShutDown = true;
			}
			break;
		case lua_shutdown:
			if(ret)
				pShutDown = true;
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
	retCode = lua_pcall(L,args,results,0);
	std::cout << "lua_pcall" << retCode << args << results << 0 << std::endl;
	if(retCode != 0)
		return false;
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
		lua_pushnumber(L,packet->getCmd());
		lua_pushlstring(L,packet->getBuff(),strlen(packet->getBuff()));
		pushPacket(L,packet);
		luaCall(L,3,0);
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
	}
	lua_settop(L,top);
}

void LuaMgr::newConnect(Packet* packet){
	std::cout << "newconnect from: " << packet->getFd() << std::endl; 
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	Packet* listen = network.getListen();
	int nRef = listen->getRef();
	lua_getref(L,nRef);
	lua_getfield(L,-1,"newConnect");
	if(lua_isfunction(L,-1)){
		// lua_pushnumber(L,packet->getFd());
		pushPacket(L,packet);
		luaCall(L,1,0);
	}
	lua_settop(L,top); 
}

int LuaMgr::getInt(const char* name){
	return getInt(pLuaState,name);
}

std::string LuaMgr::getString(const char* name){
	return getString(pLuaState,name);
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
	luamgr->setStateFunc(L,lua_load,-1);

	lua_getfield(L,1,"update")
	luamgr->setStateFunc(L,lua_update,-1);

	lua_getfield(L,1,"shutdown")
	luamgr->setStateFunc(L,lua_shutdown,-1);
	return 0;
}

int luaConnectServer(lua_State* L){
	std::cout << "start connect server"<< std::endl;
	int top = lua_gettop(L);
	const char* sIp = lua_tostring(L,1);
	int nPort = lua_tointeger(L,2);
	int fd = 0;
	fd = network.connectServer(sIp,nPort);
	if(fd > 0){
		std::cout << "connectServer success : [" << sIp<<":"<<nPort<<"] socket: "<< fd<<std::endl;
	}
	Packet* packet = network.getPacket(fd);
	pushPacket(L,packet);
	return 1;
}

int luaCreateServer(lua_State* L){
	int top = lua_gettop(L);
	const char* sIp = lua_tostring(L,1);
	int nPort = lua_tointeger(L,2);
	if(network.createServer(sIp,nPort)){
		std::cout << "createServer success : [" << sIp<<":"<<nPort<<"] "<<std::endl;
	}
	Packet* packet = network.getListen();
	pushPacket(L,packet);
	return 1;
}

int luaSendData(lua_State* L){
	int top = lua_gettop(L);
	Packet* packet = (Packet*)lua_touserdata(L,lua_upvalueindex(1));
	short cmd = (short)lua_tonumber(L,1);
	size_t length = 0;
	const char* data =lua_tolstring(L,2,&length);
	packet->sendData(length,cmd,data);
	return 0;
}
/*******************************************function to get data from lua*/

std::string getString(lua_State* L, const char * name){
	int top	= lua_gettop(L);
	lua_getglobal(L,name);
	std::string string = lua_tostring(L,-1);
	lua_settop(L,top);
	return string;
}

int getInt(lua_State* L, const char* name){
	int top	= lua_gettop(L);
	lua_getglobal(L,name);
	int result = lua_tonumber(L,-1);
	lua_settop(L,top);
	return result;	
}

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

	lua_newtable(L);

	lua_pushstring(L,"__packet");
	lua_pushlightuserdata(L,packet);
	lua_rawset(L,-3);

	pushLuaFunction("SendData",L,packet,luaSendData);

	lua_pushvalue(L,-1);
	nRef = lua_ref(L,true);
	packet->setRef(nRef);
	std::cout << "pushPacket success: " <<nRef << std::endl;
}