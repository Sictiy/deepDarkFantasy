#include "luapacker.h"

/****************************/
LuaPacker::LuaPacker(){
	buff.clear();
	tableCount = 0;
}

LuaPacker::~LuaPacker(){
	clear();
}

void LuaPacker::clear(){
	buff.clear();
}

void LuaPacker::serialize(unsigned char** data, size_t* dataLen){
	size_t size = buff.size();
	if(size > 0)
		*data = &buff[0];
	*dataLen = size;
}

bool LuaPacker::isInteger(double d, int i){
	return(fabs(d- (double)i) < 1e-10);
}

bool LuaPacker::pushValue(lua_State* L, int dataIndex, int count){
	if(dataIndex < 0)
		dataIndex = lua_gettop(L) + dataIndex + 1;
	std::cout<< "start push value count: " <<count<<std::endl;

	for(int i=0; i< count;i++){
		int index = dataIndex + i;
		int type = lua_type(L,index);
		double dvalue = 0;
		int nvalue = 0;
		bool bvalue = false;
		size_t len = 0;
		int top = 0;
		bool isarray = false;
		bool success = false;
		const char* pValue = NULL;
		// std::cout<< "type " <<type<<std::endl;
		switch(type){
			case LUA_TNUMBER:
				// std::cout<< "start push number" <<std::endl;
				dvalue = lua_tonumber(L,index);
				nvalue = (int)dvalue;
				if(isInteger(dvalue,nvalue)){
					pushByte(vInteger);
					pushData((int32_t)nvalue);
				}
				else{
					pushByte(vNumber);
					pushData((float64_t)dvalue);
				}
				break;
			case LUA_TBOOLEAN:
				// std::cout<< "start push boolean" <<std::endl;
				bvalue = lua_toboolean(L,index);
				pushByte(vBoolean);
				pushData((unsigned char)bvalue);
				break;
			case LUA_TSTRING:
				// std::cout<< "start push string" <<std::endl;
				len = 0;
				pValue = lua_tolstring(L,index,&len);
				if(len <= USHRT_MAX){
					pushByte(vString);
					pushData((uint16_t)len);
				}else{
					pushByte(vBinary);
					pushData((uint32_t)len);
				}
				pushBytes((const unsigned char*)pValue,len);
				break;
			case LUA_TNIL:
				pushByte(vNil);
				break;
			case LUA_TTABLE:
				// std::cout<< "start push table" <<std::endl;
				top = lua_gettop(L);
				lua_getmetatable(L,index);
				if(lua_istable(L,-1)){
					lua_getfield(L,-1,"__isarray");
					isarray = lua_toboolean(L,-1);
				}
				lua_settop(L,top);
				if(isarray)
					success = pushArray(L,index);
				else
					success = pushTable(L,index);
				if(!success){
					std::cout<< "push table failed!"<<std::endl;
					return false;
				}
				break;
			default:
				std::cout<< "unknown type" <<std::endl;
				lua_pushfstring(L,"Unknown type (%d) to serialize!", type);
				lua_error(L);
				return false;
		}
	}
	return true;
}

bool LuaPacker::pushArray(lua_State* L,int dataIndex){
	int top = lua_gettop(L);
	if(dataIndex < 0)
		dataIndex = top + 1 + dataIndex;

	pushByte(vArray);
	size_t arrayIndex = buff.size();
	pushData((uint32_t)0);

	int keyIndex = 0;
	tableCount++;
	while(true) {
		lua_rawgeti(L,dataIndex,++keyIndex);
		if(lua_isnil(L,-1))
			break;
		if(pushValue(L,-1)){
			lua_settop(L,top);
			tableCount--;
			return false;
		}
		lua_pop(L,1);
	};

	size_t bufflen = buff.size();
	size_t tablelen = bufflen - arrayIndex - sizeof(uint32_t);
	*(uint32_t*)&buff[arrayIndex] = (uint32_t)tablelen;

	return true;
}

bool LuaPacker::pushTable(lua_State* L,int dataIndex){
	int top = lua_gettop(L);
	if(dataIndex < 0)
		dataIndex = top + 1 + dataIndex;

	pushByte(vTable);
	size_t tableIndex = buff.size();
	pushData((uint32_t)0);

	tableCount++;
	lua_pushnil(L);
	while(lua_next(L,dataIndex)) {
		int topIndex = lua_gettop(L);
		if(!(pushValue(L,topIndex - 1)&&pushValue(L,topIndex))){
			lua_settop(L,top);
			tableCount--;
			return false;
		}
		lua_pop(L,1);
	};

	size_t bufflen = buff.size();
	size_t tablelen = bufflen - tableIndex - sizeof(uint32_t);
	*(uint32_t*)&buff[tableIndex] = (uint32_t)tablelen;

	return true;
}

int LuaPacker::unserilize(lua_State* L,const unsigned char* dataIndex, size_t dataLen){
	int count = 0;
	const unsigned char* index = dataIndex;
	const unsigned char* end = dataIndex + dataLen;
	std::cout << "start unserilize count: "<<dataLen <<"index : " <<dataIndex<<std::endl;
	while(index < end) {
	    index = usValue(L, index, (size_t)(end - index));
	    if(index == NULL)
	    	return count;
	    count++;
	}
	return count;
}

const unsigned char* LuaPacker::usValue(lua_State* L, const unsigned char* dataIndex, size_t dataLen){
	const unsigned char* ret = NULL;
	const unsigned char* index = dataIndex;
	const unsigned char* end = dataIndex + dataLen;
	int type = 0;
	if((size_t)(end - index) < sizeof(unsigned char)){
		return ret;
	}
	type = *index++;

	int value = 0;
	double dvalue = 0;
	size_t len = 0;
	bool bvalue = false;
	uint32_t size = 0;
	switch(type){
		case vInteger:
			if((size_t)(end - index) < sizeof(int32_t))
				return ret;
			value = *(int32_t*)index;
			index += sizeof(int32_t);
			lua_pushinteger(L,value);
			// std::cout<< "get integer" <<std::endl;
			break;
		case vNumber:
			if((size_t)(end - index) < sizeof(float64_t))
				return ret;
			dvalue = (double)(*(float64_t*)index);
			index += sizeof(float64_t);
			lua_pushnumber(L,dvalue);
			// std::cout<< "get number" <<std::endl;
			break;
		case vString:
			if((size_t)(end - index) < sizeof(uint16_t))
				return ret;
			len = (size_t)*(uint16_t*)index;
			index += sizeof(uint16_t);
			if((size_t)(end - index) < len)
				return ret;
			lua_pushlstring(L, (const char*)index, len);
			index += len;
			// std::cout<< "get string" <<std::endl;
			break;
		case vBinary:
			if((size_t)(end - index) < sizeof(uint32_t))
				return ret;
			len = (size_t)*(uint32_t*)index;
			index += sizeof(uint32_t);
			if((size_t)(end - index) < sizeof(uint32_t))
				return ret;
			lua_pushlstring(L,(const char*)index,len);
			index += len;
			// std::cout<< "get binary" <<std::endl;
			break;
		case vNil:
			lua_pushnil(L);
			// std::cout<< "get nil" <<std::endl;
			break;
		case vBoolean:
			if((size_t)(end - index) < sizeof(unsigned char))
				return ret;
			bvalue = (*index != 0);
			index += sizeof(unsigned char);
			lua_pushboolean(L,bvalue);
			// std::cout<< "get boolean" <<std::endl;
			break;
		case vArray:
			if((size_t)(end - index) < sizeof(unsigned char))
				return ret;
			size = *(uint32_t*)index;
			index += sizeof(uint32_t);
			if((size_t)(end - index) < size)
				return ret;
			if(!usArray(L,index,size))
				return ret;
			index += size;
			// std::cout<< "get array" <<std::endl;
			break;
		case vTable:
			if((size_t)(end - index) < sizeof(unsigned char))
				return ret;
			size = *(uint32_t*)index;
			index += sizeof(uint32_t);
			if((size_t)(end - index) < size)
				return ret;
			if(!usTable(L,index,size))
				return ret;
			index += size;
			// std::cout<< "get table" <<std::endl;
			break;
		default:
			std::cout<< "unknow type " <<std::endl;
			return ret;
	}
	return index;
}

bool LuaPacker::usArray(lua_State* L, const unsigned char* dataIndex, size_t dataLen){
	const unsigned char* index = dataIndex;
	const unsigned char* end = index + dataLen;
	int arrayIndex = 0;
	lua_newtable(L);
	while(index < end) {
		index = usValue(L,index,(size_t)(end - index));
		if(index == NULL)
			return false;
		lua_rawseti(L,-2,++arrayIndex);
	};

	int top = lua_gettop(L);
	if(!lua_istable(L,top))
		return false;
	if(lua_setmetatable(L,top) ==0 ){
		lua_newtable(L);
		lua_pushvalue(L,-1);
		lua_setmetatable(L,top);
	}
	lua_pushboolean(L,true);
	lua_setfield(L,-2,"__isarray");
	lua_settop(L,top);

	return true;
}

bool LuaPacker::usTable(lua_State* L, const unsigned char* dataIndex, size_t dataLen){
	const unsigned char* index = dataIndex;
	const unsigned char* end = index + dataLen;
	lua_newtable(L);
	while(index < end) {
	    index = usValue(L,index,(size_t)(end - index));
	    if(index == NULL)
	    	return false;
	    index = usValue(L,index,(size_t)(end - index));
	    if(index == NULL)
	    	return false;
	    lua_rawset(L,-3);
	};
	return true;
}

void LuaPacker::pushByte(valueType byte){
	buff.push_back(byte);
	// std::cout << "push byte:"<< byte << std::endl;
}

void LuaPacker::pushBytes(const unsigned char* bytes, size_t len){
	if(len == 0)
		return;
	size_t index = buff.size();
	buff.resize(index+len);
	memcpy(&buff[index],bytes,len);
	// std::cout << "push bytes:"<< std::endl;
}

template<typename T>
void LuaPacker::pushData(const T& value){
    const unsigned char* pValue = (const unsigned char*)&value;
    size_t len = sizeof(T);
    pushBytes(pValue, len);
}
