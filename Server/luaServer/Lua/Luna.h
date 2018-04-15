#pragma once

#ifdef LUAPLUS_DLL
    #ifdef LUAPLUS_EXPORT
        #define LUAPLUS_API DLL_EXPORT
    #else
        #define LUAPLUS_API DLL_IMPORT
    #endif
#else
    #define LUAPLUS_API
#endif

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

enum LuaObjectMemberType
{
    eLuaObjectMemberType_Invalid,
    eLuaObjectMemberType_int,
    eLuaObjectMemberType_enum,
    eLuaObjectMemberType_DWORD,
    eLuaObjectMemberType_WORD,
    eLuaObjectMemberType_BYTE,
    eLuaObjectMemberType_time,
    eLuaObjectMemberType_small_bool,
    eLuaObjectMemberType_big_bool,
    eLuaObjectMemberType_float,
    eLuaObjectMemberType_double,
    eLuaObjectMemberType_string,
    eLuaObjectMemberType_std_str,
    eLuaObjectMemberType_function
};

#define LUA_OBJECT_POINTER "__obj_pointer__"

template <typename T>
int LuaFunctionAdpter(lua_State* L)
{
    T*                              pObj       = (T*)lua_touserdata(L, lua_upvalueindex(1));
    typename T::_ObjectMemberInfo*  pMember    = (typename T::_ObjectMemberInfo*)lua_touserdata(L, lua_upvalueindex(2));

    if (pObj == NULL || pMember == NULL || pMember->GetFunc == NULL)
        return 0;

    return (pObj->*(typename T::LUA_MEMBER_FUNC)pMember->GetFunc)(L);
}

template <typename T> 
int LuaIndex(lua_State* L)
{
    int                             nResult         = 0;
    int                             nRetCode        = 0;
    int                             nTopIndex       = 0;
    T*                              pObj            = NULL;
    const char*                     pszKey          = NULL;
    typename T::_ObjectMemberInfo*  pMemberInfo     = NULL;
    void*                           pvAddr          = NULL;   
    std::string*                    pStdStr         = NULL;

    nTopIndex = lua_gettop(L);
    X_FAILED_JUMP(nTopIndex == 2);
    
    lua_pushstring(L, LUA_OBJECT_POINTER);
	lua_rawget(L, 1);

    pObj = (T*)lua_touserdata(L, -1);
	if (pObj == NULL)
	{
		lua_pushstring(L, "[Luna] C-obj reference invalid, LuaIndex failed!");
		lua_error(L);
		goto Exit0;
	}

    pszKey = lua_tostring(L, 2);
    X_FAILED_JUMP(pszKey);

    lua_getglobal(L, T::_ms_pszMetaTableName);
    X_FAILED_JUMP(lua_istable(L, -1));

    lua_pushstring(L, pszKey);
    lua_rawget(L, -2);

	if (lua_isfunction(L, -1))
	{
		lua_pushvalue(L, 1);
		lua_setupvalue(L, -2, 1);
		goto Exit1;
	}
    X_FAILED_JUMP(lua_isuserdata(L, -1));
    pMemberInfo = (typename T::_ObjectMemberInfo*)lua_touserdata(L, -1);
    X_FAILED_JUMP(pMemberInfo);

    pvAddr = ((BYTE*)pObj) + pMemberInfo->nOffset;

	if (pMemberInfo->GetFunc)
	{
		switch (pMemberInfo->nType)
		{
		case eLuaObjectMemberType_int:
		case eLuaObjectMemberType_enum:
		case eLuaObjectMemberType_time:
			lua_pushinteger(L, (pObj->*(typename T::LUA_GETINT_FUNC)pMemberInfo->GetFunc)());
			break;

		case eLuaObjectMemberType_DWORD:
		case eLuaObjectMemberType_WORD:
		case eLuaObjectMemberType_BYTE:
			lua_pushinteger(L, (pObj->*(typename T::LUA_GETDWORD_FUNC)pMemberInfo->GetFunc)());
			break;

		case eLuaObjectMemberType_string:
			lua_pushstring(L, (pObj->*(typename T::LUA_GETSTR_FUNC)pMemberInfo->GetFunc)());
			break;

		case eLuaObjectMemberType_function:
			lua_pushlightuserdata(L, pObj);
			lua_pushlightuserdata(L, pMemberInfo);
			lua_pushcclosure(L, LuaFunctionAdpter<T>, 2);
			break;

		default:
			lua_pushnil(L);
		}
	}
	else
	{
		switch (pMemberInfo->nType)
		{
		case eLuaObjectMemberType_int:
		case eLuaObjectMemberType_enum:
		case eLuaObjectMemberType_DWORD:
			assert(pMemberInfo->uSize == sizeof(int));
			lua_pushinteger(L, *(int*)pvAddr);
			break;

		case eLuaObjectMemberType_WORD:
			assert(pMemberInfo->uSize == sizeof(WORD));
			lua_pushinteger(L, *(WORD*)pvAddr);
			break;

		case eLuaObjectMemberType_BYTE:
			assert(pMemberInfo->uSize == sizeof(BYTE));
			lua_pushinteger(L, *(BYTE*)pvAddr);
			break;

		case eLuaObjectMemberType_time:
			assert(pMemberInfo->uSize == sizeof(time_t));
			lua_pushnumber(L, (double)*(time_t*)pvAddr);
			break;

		case eLuaObjectMemberType_small_bool:
			assert(pMemberInfo->uSize == sizeof(bool));
			lua_pushboolean(L, *(bool*)pvAddr);
			break;

		case eLuaObjectMemberType_big_bool:
			assert(pMemberInfo->uSize == sizeof(BOOL));
			lua_pushboolean(L, *(BOOL*)pvAddr);
			break;

		case eLuaObjectMemberType_float:
			assert(pMemberInfo->uSize == sizeof(float));
			lua_pushnumber(L, *(float*)pvAddr);
			break;

		case eLuaObjectMemberType_double:
			assert(pMemberInfo->uSize == sizeof(double));
			lua_pushnumber(L, *(double*)pvAddr);
			break;

		case eLuaObjectMemberType_string:
			lua_pushstring(L, (const char*)pvAddr);
			break;

		case eLuaObjectMemberType_std_str:
			pStdStr = (std::string*)pvAddr;
			lua_pushstring(L, pStdStr->c_str());
			break;

		default:
			lua_pushnil(L);
		}
	}

Exit1:
    nResult = 1;
Exit0:
    return nResult;
}


template <typename T>
int LuaNewIndex(lua_State* L)
{
    int                             nRetCode        = 0;
    int                             nTopIndex       = 0;
    T*                              pObj            = NULL;
    const char*                     pszKey          = NULL;
    const char*                     pszStr          = NULL;
    typename T::_ObjectMemberInfo*  pMemberInfo     = NULL;
    void*                           pvAddr          = NULL;
    size_t                          uStrLen         = 0;

    nTopIndex = lua_gettop(L);
    X_FAILED_JUMP(nTopIndex == 3);
    
    lua_pushstring(L, LUA_OBJECT_POINTER);
    lua_rawget(L, 1);

    pObj = (T*)lua_touserdata(L, -1);
	if (pObj == NULL)
	{
		lua_pushstring(L, "[Luna] C-obj reference invalid, LuaNewIndex failed!");
		lua_error(L);
		goto Exit0;
	}

    pszKey = lua_tostring(L, 2);
    X_FAILED_JUMP(pszKey);

    lua_getglobal(L, T::_ms_pszMetaTableName);
    X_FAILED_JUMP(lua_istable(L, -1));

    lua_pushvalue(L, 2);
    lua_rawget(L, -2);

    pMemberInfo = (typename T::_ObjectMemberInfo*)lua_touserdata(L, -1);
    if (pMemberInfo == NULL)
    {
		lua_settop(L, 3);
        lua_rawset(L, 1);
        goto Exit1;
    }

    if(pMemberInfo->bReadOnly)
    {
		lua_pushfstring(L, "[Luna] Try to write member readonly: %s::%s\n", T::_ms_pszClassName, pMemberInfo->pszName);
		lua_error(L);
        goto Exit0;
    }

    pvAddr = ((BYTE*)pObj) + pMemberInfo->nOffset;
	if (pMemberInfo->SetFunc)
	{
		switch (pMemberInfo->nType)
		{
		case eLuaObjectMemberType_int:
		case eLuaObjectMemberType_enum:
		case eLuaObjectMemberType_time:
			(pObj->*(typename T::LUA_SETINT_FUNC)pMemberInfo->SetFunc)((int)lua_tointeger(L, 3));
			break;

		case eLuaObjectMemberType_DWORD:
		case eLuaObjectMemberType_WORD:
		case eLuaObjectMemberType_BYTE:
			(pObj->*(typename T::LUA_SETDWORD_FUNC)pMemberInfo->SetFunc)((DWORD)lua_tointeger(L, 3));
			break;

		case eLuaObjectMemberType_string:
			(pObj->*(typename T::LUA_SETSTR_FUNC)pMemberInfo->SetFunc)(lua_tostring(L, 3));
			break;

		default:
			lua_pushnil(L);
		}
	}
	else
	{
		switch (pMemberInfo->nType)
		{
		case eLuaObjectMemberType_int:
		case eLuaObjectMemberType_enum:
		case eLuaObjectMemberType_DWORD:
			assert(pMemberInfo->uSize == sizeof(int));
			*(int*)pvAddr = (int)lua_tointeger(L, 3);
			break;

		case eLuaObjectMemberType_WORD:
			assert(pMemberInfo->uSize == sizeof(WORD));
			*(WORD*)pvAddr = (WORD)lua_tointeger(L, 3);
			break;

		case eLuaObjectMemberType_BYTE:
			assert(pMemberInfo->uSize == sizeof(BYTE));
			*(BYTE*)pvAddr = (BYTE)lua_tointeger(L, 3);
			break;

		case eLuaObjectMemberType_time:
			assert(pMemberInfo->uSize == sizeof(time_t));
			*(time_t*)pvAddr = (time_t)lua_tonumber(L, 3);
			break;

		case eLuaObjectMemberType_small_bool:
			assert(pMemberInfo->uSize == sizeof(bool));
			*(bool*)pvAddr = !!lua_toboolean(L, 3);
			break;

		case eLuaObjectMemberType_big_bool:
			assert(pMemberInfo->uSize == sizeof(BOOL));
			*(BOOL*)pvAddr = lua_toboolean(L, 3);
			break;

		case eLuaObjectMemberType_float:
			assert(pMemberInfo->uSize == sizeof(float));
			*(float*)pvAddr = (float)lua_tonumber(L, 3);
			break;

		case eLuaObjectMemberType_double:
			assert(pMemberInfo->uSize == sizeof(double));
			*(double*)pvAddr = lua_tonumber(L, 3);
			break;

		case eLuaObjectMemberType_string:
			X_FAILED_JUMP(lua_isstring(L, 3));
			pszStr = (const char*)lua_tostring(L, 3);
			X_FAILED_JUMP(pszStr);
			uStrLen = strlen(pszStr);
			X_FAILED_JUMP(uStrLen < pMemberInfo->uSize);
			strcpy((char*)pvAddr, pszStr);
			break;

		case eLuaObjectMemberType_std_str:
			X_FAILED_JUMP(lua_isstring(L, 3));
			pszStr = (const char*)lua_tostring(L, 3);
			X_FAILED_JUMP(pszStr);
			*(std::string*)pvAddr = pszStr;
			break;

		default:
			goto Exit0;
		}
	}

Exit1:
Exit0:
    return 0;
}

template <typename T>
void LuaRegisterClass(lua_State* L)
{
    int         nTopIndex               = lua_gettop(L);
    const char* pszMemberVarPrefix      = "m_";
    size_t      uMemberVarPrefixLen     = strlen(pszMemberVarPrefix);
    const char* pszMemberFuncPrefix     = "Lua";
    size_t      uMemberFuncPrefixLen    = strlen(pszMemberFuncPrefix);

	lua_newtable(L);

    typename T::_ObjectMemberInfo* pMember = T::GetMemberList();
    while (pMember->nType != eLuaObjectMemberType_Invalid)
    {
        const char* pszName  = pMember->pszName;
        size_t      uNameLen = strlen(pszName);

        if (pMember->nType == eLuaObjectMemberType_function)
        {
            if (uNameLen > uMemberFuncPrefixLen)
            {   int nCmp = memcmp(pszName, pszMemberFuncPrefix, uMemberFuncPrefixLen);
                if (nCmp == 0)
                {
                    pszName += uMemberFuncPrefixLen;
                }
            }
        }
        else
        {
            if (uNameLen > uMemberVarPrefixLen)
            {
            	int nCmp = memcmp(pszName, pszMemberVarPrefix, uMemberVarPrefixLen);
                if (nCmp == 0)
                {
                    pszName += uMemberVarPrefixLen;
                }
            }
        }

        lua_pushstring(L, pszName);
        lua_pushlightuserdata(L, pMember);
        lua_settable(L, -3);
        
        pMember++;
    }

	lua_pushvalue(L, -1);
	lua_setglobal(L, T::_ms_pszMetaTableName);

	luaL_newmetatable(L, T::_ms_pszMetaTableName);
	lua_pushvalue(L, -2);
	lua_setfield(L, -2, "__metatable");

	lua_pushcfunction(L, (&LuaIndex<T>));
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, (&LuaNewIndex<T>));
	lua_setfield(L, -2, "__newindex");

    lua_settop(L, nTopIndex);
}

struct XLuaObjRef
{
    XLuaObjRef()
    {
        m_pLuaState = NULL;
        m_nLuaRef = LUA_NOREF;
    }

    ~XLuaObjRef()
    {
        Clear();
    }
    void Clear()
    {
        int nTop = 0;

        if (m_pLuaState)
        {
            nTop = lua_gettop(m_pLuaState);
        }

        X_FAILED_JUMP(m_nLuaRef != LUA_NOREF);

        lua_rawgeti(m_pLuaState, LUA_REGISTRYINDEX, m_nLuaRef);
        X_FAILED_JUMP(lua_istable(m_pLuaState, -1));

        lua_pushstring(m_pLuaState, LUA_OBJECT_POINTER);
        lua_pushnil(m_pLuaState);
        lua_settable(m_pLuaState, -3);

        luaL_unref(m_pLuaState, LUA_REGISTRYINDEX, m_nLuaRef);

Exit0:
        if (m_pLuaState)
        {
            lua_settop(m_pLuaState, nTop);
        }

        m_pLuaState = NULL;
        m_nLuaRef = LUA_NOREF;
    }
    lua_State*  m_pLuaState;
    int         m_nLuaRef;
};

#define DECLARE_LUA_CLASS(ClassName)    \
	typedef void (ClassName::*LUA_VOID_FUNC)(void) const; \
	typedef void (ClassName::*LUA_SETINT_FUNC)(int); \
	typedef int (ClassName::*LUA_GETINT_FUNC)(void) const; \
	typedef void (ClassName::*LUA_SETSTR_FUNC)(const char*); \
	typedef const char* (ClassName::*LUA_GETSTR_FUNC)(void) const; \
	typedef void (ClassName::*LUA_SETDWORD_FUNC)(DWORD); \
	typedef DWORD (ClassName::*LUA_GETDWORD_FUNC)(void) const; \
	typedef int (ClassName::*LUA_MEMBER_FUNC)(lua_State*); \
    struct _ObjectMemberInfo    \
    {   \
		int             nType;      \
		const char*     pszName;    \
		int             nOffset;    \
		size_t          uSize;      \
		BOOL            bReadOnly;  \
        LUA_VOID_FUNC	GetFunc;   \
		LUA_VOID_FUNC	SetFunc;   \
    };  \
    XLuaObjRef  _m_LuaRef;  \
    static const char* _ms_pszClassName;    \
    static const char* _ms_pszMetaTableName;    \
    static ClassName::_ObjectMemberInfo* GetMemberList();   \
    \
    virtual void LuaPushThis(lua_State* L)  \
    {   \
        if (_m_LuaRef.m_nLuaRef != LUA_NOREF)   \
        {   \
            lua_rawgeti(L, LUA_REGISTRYINDEX, _m_LuaRef.m_nLuaRef); \
            return; \
        }   \
        \
        lua_newtable(L);    \
        lua_pushstring(L, LUA_OBJECT_POINTER);  \
        lua_pushlightuserdata(L, this); \
        lua_settable(L, -3);    \
        \
        luaL_getmetatable(L, _ms_pszMetaTableName); \
        if (lua_isnil(L, -1)) \
        {   \
            lua_remove(L, -1);  \
            LuaRegisterClass<ClassName>(L); \
            luaL_getmetatable(L, _ms_pszMetaTableName); \
        }   \
        lua_setmetatable(L, -2);    \
        \
        lua_pushvalue(L, -1);   \
        _m_LuaRef.m_pLuaState = L;  \
        _m_LuaRef.m_nLuaRef = luaL_ref(L, LUA_REGISTRYINDEX);  \
    }

#define LUA_CLEAR_REF() _m_LuaRef.Clear()

#define IMPL_LUA_CLASS_BEGIN(ClassName) \
    const char* ClassName::_ms_pszClassName = #ClassName;   \
    const char* ClassName::_ms_pszMetaTableName = "_"#ClassName;  \
ClassName::_ObjectMemberInfo* ClassName::GetMemberList()    \
{   \
    typedef ClassName   __ThisClass__;  \
    static _ObjectMemberInfo _ms_MemberList[] =  \
    {

#define IMPL_LUA_CLASS_END()    \
        {eLuaObjectMemberType_Invalid, NULL, 0, 0, false, NULL},    \
    };  \
    return _ms_MemberList;  \
}

#define EXPORT_LUA_MEMBER(Type, Member, bReadOnly)  \
    {Type, #Member, offsetof(__ThisClass__, Member), sizeof(((__ThisClass__*)NULL)->Member), bReadOnly, NULL, NULL},

#define EXPORT_LUA_INT(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_int, Member, false)
#define EXPORT_LUA_INT_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_int, Member, true)

#define EXPORT_LUA_ENUM(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_enum, Member, false)
#define EXPORT_LUA_ENUM_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_enum, Member, true)

#define EXPORT_LUA_DWORD(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_DWORD, Member, false)
#define EXPORT_LUA_DWORD_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_DWORD, Member, true)

#define EXPORT_LUA_WORD(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_WORD, Member, false)
#define EXPORT_LUA_WORD_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_WORD, Member, true)

#define EXPORT_LUA_BYTE(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_BYTE, Member, false)
#define EXPORT_LUA_BYTE_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_BYTE, Member, true)

#define EXPORT_LUA_TIME(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_time, Member, false)
#define EXPORT_LUA_TIME_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_time, Member, true)

#define EXPORT_LUA_SMALL_BOOL(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_small_bool, Member, false)
#define EXPORT_LUA_SMALL_BOOL_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_small_bool, Member, true)

#define EXPORT_LUA_BIG_BOOL(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_big_bool, Member, false)
#define EXPORT_LUA_BIG_BOOL_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_big_bool, Member, true)

#define EXPORT_LUA_FLOAT(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_float, Member, false)
#define EXPORT_LUA_FLOAT_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_float, Member, true)

#define EXPORT_LUA_DOUBLE(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_double, Member, false)
#define EXPORT_LUA_DOUBLE_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_double, Member, true)

#define EXPORT_LUA_STRING(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_string, Member, false)
#define EXPORT_LUA_STRING_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_string, Member, true)

#define EXPORT_LUA_STD_STR(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_std_str, Member, false)
#define EXPORT_LUA_STD_STR_R(Member)   EXPORT_LUA_MEMBER(eLuaObjectMemberType_std_str, Member, true)

#define EXPORT_LUA_FUNCTION(Member) \
    {eLuaObjectMemberType_function, #Member, 0, 0, true, (LUA_VOID_FUNC)&__ThisClass__::Member, NULL},

#define EXPORT_LUA_PROPERTY(Type, Prefix, Property) \
	{Type, #Prefix#Property, 0, 0, false, (LUA_VOID_FUNC)&__ThisClass__::get##Property, (LUA_VOID_FUNC)&__ThisClass__::set##Property},

#define EXPORT_LUA_PROPERTY_R(Type, Prefix, Property) \
	{Type, #Prefix#Property, 0, 0, true, (LUA_VOID_FUNC)&__ThisClass__::get##Property, NULL},

#define EXPORT_LUA_PROPERTY_INT(Property) EXPORT_LUA_PROPERTY(eLuaObjectMemberType_int, n, Property)
#define EXPORT_LUA_PROPERTY_INT_R(Property) EXPORT_LUA_PROPERTY_R(eLuaObjectMemberType_int, n, Property)

#define EXPORT_LUA_PROPERTY_DWORD(Property) EXPORT_LUA_PROPERTY(eLuaObjectMemberType_DWORD, dw, Property)
#define EXPORT_LUA_PROPERTY_DWORD_R(Property) EXPORT_LUA_PROPERTY_R(eLuaObjectMemberType_DWORD, dw, Property)

#define EXPORT_LUA_PROPERTY_STRING(Property) EXPORT_LUA_PROPERTY(eLuaObjectMemberType_string, sz, Property)
#define EXPORT_LUA_PROPERTY_STRING_R(Property) EXPORT_LUA_PROPERTY_R(eLuaObjectMemberType_string, sz, Property)

#define EXPORT_LUA_PROPERTY_ENUM(Property) EXPORT_LUA_PROPERTY(eLuaObjectMemberType_enum, e, Property)
#define EXPORT_LUA_PROPERTY_ENUM_R(Property) EXPORT_LUA_PROPERTY_R(eLuaObjectMemberType_enum, e, Property)

#define EXPORT_LUA_PROPERTY_BOOL(Property) EXPORT_LUA_PROPERTY(eLuaObjectMemberType_int, b, Property)
#define EXPORT_LUA_PROPERTY_BOOL_R(Property) EXPORT_LUA_PROPERTY_R(eLuaObjectMemberType_int, b, Property)

template <typename T>
void Lua_PushObject(lua_State* L, T* pObj)
{
    if (pObj == NULL)
    {
        lua_pushnil(L);
        return;
    }
    pObj->LuaPushThis(L);
}

template <typename T>
T* Lua_ToObject(lua_State* L, int idx)
{
    T* pObj = NULL;
    if (lua_istable(L, idx))
    {
        lua_getfield(L, idx, LUA_OBJECT_POINTER);
        pObj = (T*)lua_touserdata(L, -1);
        lua_pop(L, 1);
    }
    return pObj;
}

template <typename T>
BOOL Lua_GetObjFunction(lua_State* L, T* pObj, const char cszFunc[])
{
    Lua_PushObject(L, pObj);
    lua_pushstring(L, cszFunc);
    lua_gettable(L, -2);
    lua_remove(L, -2);

    return (lua_type(L, -1) == LUA_TFUNCTION);
}

struct XLuaConstValue
{
    const char* pszName;
    lua_Number  Value;
};

LUAPLUS_API DWORD Lua_FileNameHash(const wchar_t* pszStr);
LUAPLUS_API const wchar_t* Lua_GetScriptName(DWORD dwScriptID);
LUAPLUS_API BOOL Lua_GetEnvName(DWORD dwScriptID, char* pszEnvName, size_t uEnvLen);

LUAPLUS_API BOOL  Lua_SetupEnv(lua_State* L, const wchar_t* pszRootDir = NULL);

LUAPLUS_API int   Lua_SearchScript(lua_State* L, const wchar_t wszDir[], BOOL bLoadScript);

LUAPLUS_API void  Lua_ExportConst(lua_State* L, const char cszName[], XLuaConstValue* pExport, int nCount);
LUAPLUS_API BOOL  Lua_LoadScript(lua_State* L, const wchar_t cszFileName[]);
LUAPLUS_API BOOL  Lua_ExecuteString(lua_State* L, const char cszEnvName[], const char cszContent[]);
LUAPLUS_API void  Lua_FreeScript(lua_State* L, const wchar_t cszFileName[]);
LUAPLUS_API BOOL  Lua_IsScriptLoaded(lua_State* L, const wchar_t cszFileName[]);
LUAPLUS_API BOOL  Lua_GetFunction(lua_State* L, const wchar_t cszFileName[], const char cszFunction[]);
LUAPLUS_API BOOL  Lua_GetFunction(lua_State* L, DWORD dwScriptID, const char cszFunction[]);
LUAPLUS_API BOOL  Lua_GetTableFunction(lua_State* L, const char cszTable[], const char cszFunction[]);
LUAPLUS_API BOOL  Lua_XCall(lua_State* L, int nArgs, int nResults);
