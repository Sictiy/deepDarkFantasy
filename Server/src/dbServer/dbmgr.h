#ifndef __DBMGR__
#define __DBMGR__

#include "../baseServer/head.h"

class Dbmgr
{
public:
	Dbmgr();
	static Dbmgr &Instance(){
		static Dbmgr inst_;
		return inst_;
	}
	virtual ~Dbmgr();

	bool init();

	void select(int fd);
	void insert(Msg * msg);
	

private:
	MYSQL_RES* queryAllScore(std::vector<RoleData>*);
	int getRow();
	int getClumn();
	int setRowAndClumn(MYSQL_RES *ptr);
	std::string queryScore(std::string name);
	bool insertScore(std::string name, std::string score);

private:
	
	MYSQL con;
	MYSQL_RES *res_ptr; //指向查询结果的指针
 	MYSQL_FIELD *field; //字段结构指针
 	MYSQL_ROW result_row; //按行返回的查询信息
 	int row, column; //查询返回的行数和列数*/

	/* data */
};

#define dbMgr Dbmgr::Instance()

#endif