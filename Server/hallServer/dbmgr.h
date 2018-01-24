#ifndef __DBMGR__
#define __DBMGR__

#include <string.h>

#include "client.h"

class Client;
typedef enum Cmd_id{
	Insert,
	Select
}Cmd_id;
typedef struct Cmd
{
	Client *client;
	Cmd_id cmd_id;
	std::string cmd;
	const std::function<void(std::vector<RoleData>&)> func;
}Cmd;

class Dbmgr
{
public:
	Dbmgr();
	virtual ~Dbmgr();

	bool init();
	void pushRequest(Cmd cmd);
	bool run();

private:
	void processRequest();
	void process();

	/* data */
};

#endif