#include "dbmgr.h"

Dbmgr::Dbmgr(){
	cmds.clear();
	RoleDataList.clear();
}

Dbmgr::~Dbmgr(){
	if(nullptr != Thread){
		Thread->join();
	}
	delete Thread;
	//close dbconnect
}

bool Dbmgr::init(){
	//connect to mysql
	mysql_init(&con);
	//mysql_query(&con, "set names utf8");//支持中文

	if(mysql_real_connect(&con, m_ip, m_mysql_usr, m_mysql_pw, m_database, 0, NULL, CLIENT_FOUND_ROWS)){
		std::cout << "connect to db success!"<<std::endl;
	}else{
		std::cout << "connect to db failed!"<<std::endl;
	}
}

void Dbmgr::run(){
	std::cout << "dbserver is runing!" << std::endl;
	Thread = new std::thread(&Dbmgr::process,this);
	Thread->detach();
}

void Dbmgr::pushRequest(Cmd cmd){
	cmds.push_front(cmd);
}

void Dbmgr::process(){
	using namespace std::chrono;
	while(true){
		if(cmds.empty())
			continue;
		else{
			Cmd cmd = cmds.back();
			cmds.pop_back();
			std::cout << "get a db cmd" <<cmd.cmd_id <<std::endl;
			processRequest(cmd);
		}
		std::this_thread::sleep_for(milliseconds(100));
	}
}

void Dbmgr::processRequest(Cmd cmd){
	//process cmd;


	//need to change
	if(cmd.cmd_id == Select){
		//select all
		RoleDataList.clear();
		queryAllScore();
		//
		respond(cmd);
	}else if(cmd.cmd_id == Insert){
		RoleData role = cmd.data;
		RoleDataList.push_back(role);
		// insert into db
		insertScore(std::string(role.name),std::to_string(role.score));
	}else{
		std::cout << "error cmd_id"<<std::endl;
	}
}

void Dbmgr::respond(Cmd cmd){
	std::cout << " dbmgr:respond respond. listen:"<<RoleDataList.size()<<std::endl;

	cmd.client->pushRespond(RoleDataList);
}

/**************************************************************************/
MYSQL_RES* Dbmgr::queryAllScore(){
	std::string str = "select * from user order by score desc;";/**/
	if(!mysql_query(&con, str.c_str())){
		res_ptr = mysql_store_result(&con);
		if(res_ptr){
			setRowAndClumn(res_ptr);

		int l = mysql_num_fields(res_ptr);
		int r = mysql_num_rows(res_ptr) + 1;

		for(int i = 0; field = mysql_fetch_field(res_ptr); ++i)
		std::cout << field->name << "  ";
		std::cout << std::endl;

		for(int i = 1; i < r; i++){
			result_row = mysql_fetch_row(res_ptr);
			RoleData role;
			for (int j = 0; j < l; ++j)
			{
				std::cout << result_row[j] << "  ";
			}
			role.name = result_row[0];
			role.score = std::stoi(result_row[1]);
			RoleDataList.push_back(role);
			std::cout << std::endl;
		}
			return res_ptr;
		}
	}

	return NULL;
}

std::string Dbmgr::queryScore(std::string name){
	std::cout << "1" <<std::endl;
	std::string str = "select score from user where name = '"+ name +"';";
	if(!mysql_query(&con, str.c_str())){//为0成功
		std::cout << "2" <<std::endl;
		res_ptr = mysql_store_result(&con);
		if(res_ptr != NULL)  {
			if (mysql_num_rows(res_ptr) == 0)
					return "null";
			std::cout << "4" <<std::endl;
			result_row = mysql_fetch_row(res_ptr);
			return result_row[0];
		}

	}
	std::cout << "3" <<std::endl;
	return "null";
}

bool Dbmgr::insertScore(std::string name, std::string score){
	std::cout << "dbmgr : insert score" <<name<<"---"<<score<<std::endl;
	std::string str = "insert into user values('"+ name +"','"+ score +"');";
	return !mysql_query(&con, str.c_str());
}

int Dbmgr::getRow(){
	return row;
}

int Dbmgr::getClumn(){
	return column;
}

int Dbmgr::setRowAndClumn(MYSQL_RES *ptr){
	if(!ptr) return -1;

	column = mysql_num_fields(ptr);
 	row = mysql_num_rows(ptr) + 1;
 	return 1;
}