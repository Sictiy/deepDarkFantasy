#include "luamgr.h"
// #include "../baseServer/packet.h"
// #include "../baseServer/msgmgr.h"
// #include "../baseServer/network.h"
// #include "msgmgr.h"
#include "network.h"

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


	/***************************/
	lua_pushcfunction(L, luaLog);
    lua_setglobal(L, "Log");

    lua_pushcfunction(L,luaCreateServer);
    lua_setglobal(L, "CreateServer");

    lua_pushcfunction(L,luaConnectServer);
    lua_getglobal(L,"ConnectServer");

	bool retCode = loadScript("config.lua","Config");
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
	// std::cout << "lua_pcall" << retCode << args << results << 0 << std::endl;
	if(retCode != 0)
		return false;
	return true;
}

bool LuaMgr::start(){
	return loadScript("main.lua");
}

/*********************************************function to network*/
void LuaMgr::recvData(Packet * packet){
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
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	int nRef = packet->getRef();
	lua_getref(L,nRef);
	lua_getfield(L,-1,"newConnect");
	if(lua_isfunction(L,-1)){
		// lua_pushnumber(L,packet->getFd());
		pushPacket(L,packet);
		luaCall(L,1,0);
	}
	lua_settop(L,top); 
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

int luaConnectServer(lua_State* L){
	std::cout << "start connect server"<< std::endl;
	int top = lua_gettop(L);
	const char* sIp = lua_tostring(L,1);
	int nPort = (int)lua_tointeger(L,2);
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