#include "luamgr.h"
// #include "../baseServer/packet.h"
// #include "../baseServer/msgmgr.h"
// #include "../baseServer/network.h"
// #include "packet.h"
// #include "msgmgr.h"
// #include "network.h"

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
	lua_pushcfunction(L, luaConnectServer);
    lua_setglobal(L, "ConnectServer");
	lua_pushcfunction(L, luaCreateServer);
    lua_setglobal(L, "CreateServer");

    // lua_newtable(L);
    // // pRef = lua_ref(L,-1);
    // lua_getglobal(L, "OnRecvData");
    // lua_setfield(L,-2,"recvData");
    // lua_getglobal(L, "OnDisConnect");
    // lua_setfield(L,-2,"disConnect");

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

void LuaMgr::recvData(Msg* msg){
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	// int nRef =  pRef;
	// lua_getref(L,nRef);
	// lua_getfield(L,-1,"recvData");
	lua_getglobal(L,"recvData");
	if(lua_isfunction(L,-1)){
		lua_pushnumber(L,msg->fd);
		lua_pushnumber(L,msg->cmd);
		lua_pushlstring(L,msg->buff,strlen(msg->buff));
		luaCall(L,3,0);
	}
	lua_settop(L,top);
}

void LuaMgr::disConnect(int fd){
	lua_State* L = pLuaState;
	int top = lua_gettop(L);
	// int nRef = pRef;
	// lua_getref(L,nRef);
	// lua_getfield(L,-1,"disConnect");
	lua_getglobal(L,"disConnect");
	if(lua_isfunction(L,-1)){
		lua_pushnumber(L,fd);
		luaCall(L,1,0);
	}
	lua_settop(L,top);
}

// void LuaMgr::test(){
// 	std::cout << "start test "<< std::endl;
// 	lua_State* L = pLuaState;
// 	int top = lua_gettop(L);
// 	lua_getglobal(L,"test");
// 	if(luaCall(L,0,1)){
// 		std::string string = lua_tostring(L,-1);
// 		std::cout << "luaCall success! " <<string<< std::endl;
// 	}
// 	lua_settop(L,top);
// 	std::cout <<"string:"<< getString(L,"string") << std::endl;
// }

/********************************************/
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
	int top = lua_gettop(L);
	const char* sIp = lua_tostring(L,1);
	int nPort = (int)lua_tointeger(L,2);
	int fd = 0;
	// fd = network.connectServer(sIp,nPort);
	// if(fd > 0){
	std::cout << "connectServer success : [" << sIp<<":"<<nPort<<"] socket: "<< fd<<std::endl;
	// 	lua_pushnumber(L,fd);
	// }
	lua_pushnumber(L,0);
	return 1;
}

int luaCreateServer(lua_State* L){
	int top = lua_gettop(L);
	const char* sIp = lua_tostring(L,1);
	int nPort = lua_tointeger(L,2);
	// if(network.createServer(sIp,nPort)){
	std::cout << "createServer success : [" << sIp<<":"<<nPort<<"] "<<std::endl;
	// 	lua_pushboolean(L,1);
	// }
	lua_pushboolean(L,0);
	return 1;
}

int luaSendData(lua_State* L){
	int top = lua_gettop(L);
	int fd = lua_tointeger(L,1);
	int cmd = lua_tointeger(L,2);
	const char* data = lua_tostring(L,3);
	// Msg* msg;
	// msg->fd = fd;
	// msg->cmd = cmd;
	// // msg->buff = const_cast<char*>(data);
	// memcpy(msg->buff,data,strlen(data));
	// Packet::sendMsg(fd,msg);
	return 0;
}
/********************************************/

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