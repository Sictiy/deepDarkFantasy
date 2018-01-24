#ifndef __DBMGR__
#define __DBMGR__

#include "client.h"

class Dbmgr
{
public:
	Dbmgr();
	virtual ~Dbmgr();

	bool init();
	static void pushRequest(Cmd cmd);
	bool run();

private:
	void processRequest(Cmd cmd);
	void process();
	void respond(Cmd cmd);

private:
	std::thread* Thread;
	static std::deque<Cmd> cmds;

	//test 
	std::vector<RoleData> RoleDataList;
	/* data */
};

#endif