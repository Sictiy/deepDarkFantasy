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
	void run();

	//new add
	
	MYSQL_RES* queryAllScore();
	int getRow();
	int getClumn();
	int setRowAndClumn(MYSQL_RES *ptr);
	std::string queryScore(std::string name);
	bool insertScore(std::string name, std::string score);

private:
	void processRequest(Cmd cmd);
	void process();
	void respond(Cmd cmd);
	//new add 

private:
	std::thread* Thread;
	std::deque<Cmd> cmds;

	//new add
	
	MYSQL con;
	MYSQL_RES *res_ptr; //指向查询结果的指针
 	MYSQL_FIELD *field; //字段结构指针
 	MYSQL_ROW result_row; //按行返回的查询信息
 	int row, column; //查询返回的行数和列数*/

	//test 
	std::vector<RoleData> RoleDataList;
	/* data */
};

#endif