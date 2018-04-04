#ifndef __LUAMGR__
#define __LUAMGR__

class LuaMgr
{
public:
	LuaMgr();
	virtual ~LuaMgr();

	bool init();
	void clear();

	bool start();
	bool loadScript(const char * script_name, const char * lua_Name);

	/* data */
};


#endif // __LUAMGR
