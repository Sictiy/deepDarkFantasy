#pragma once
#include "head.h"

extern "C"{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
};

enum valueType {
	vInteger,
	vNumber,
	vBoolean,
	vString,
	vBinary,
	vNil,
	vArray,
	vTable,
};

class LuaPacker{
public:
	LuaPacker();
	~LuaPacker();

	void serialize(unsigned char** data, size_t* dataLen);
	int unserilize(lua_State* L,const unsigned char* dataIndex, size_t dataLen);
	bool pushValue(lua_State* L, int dataIndex,int count = 1);

private:
	bool isInteger(double d, int i);
	bool pushTable(lua_State* L, int dataIndex);
	bool pushArray(lua_State* L, int dataIndex);
	bool pushNil();
	const unsigned char* usValue(lua_State*,const unsigned char* dataIndex, size_t dataLen);
	bool usArray(lua_State*, const unsigned char* dataIndex, size_t dataLen);
	bool usTable(lua_State*, const unsigned char* dataIndex, size_t dataLen);

	void pushByte(valueType);
	void pushBytes(const unsigned char* bytes, size_t len);
	template<typename T>
	void pushData(const T& value);

	std::vector<unsigned char> buff;
	int tableCount ;
};