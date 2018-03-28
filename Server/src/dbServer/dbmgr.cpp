#include "dbmgr.h"
#include "hallhandler.h"
#include "../baseServer/Protos.pb.h"

Dbmgr::Dbmgr(){
}

Dbmgr::~Dbmgr(){
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

void Dbmgr::insert(Msg *msg){
	deepdf::UserInfo *role = new deepdf::UserInfo() ;
	role->ParseFromArray(msg->buff,strlen(msg->buff));
	// std::cout <<"new role:"<< role->name()<<"--"<<role->score()<<std::endl;
 	std::cout << insertScore(std::string(role->name()),std::to_string(role->score()))  << std::endl;
}

void Dbmgr::select(int fd){
	std::vector<RoleData> RoleDataList;
	queryAllScore(&RoleDataList);
	deepdf::DataResp *respond = new deepdf::DataResp();
	for(int i=0;i<RoleDataList.size();i++){
		RoleData roledata = RoleDataList.at(i);
		deepdf::UserInfo *role = respond->add_users();
		role->set_name(roledata.name);
		role->set_score(roledata.score); 
	}
	int length = respond->ByteSize();
	char* data = new char[length];
	bzero(data,length);
	respond->SerializeToArray(data,length);
	std::cout << "count of role:"<< RoleDataList.size() <<" length:"<<length<< std::endl;
	HallHandler::sendData(fd,d2h_get,data);
}
/**************************************************************************/
MYSQL_RES* Dbmgr::queryAllScore(std::vector<RoleData> *RoleDataList){
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
			RoleDataList->push_back(role);
			std::cout << std::endl;
		}
			return res_ptr;
		}
	}
	std::cout << "select success" << std::endl;
	return NULL;
}

std::string Dbmgr::queryScore(std::string name){
	//std::cout << "1" <<std::endl;
	std::string str = "select score from user where name = '"+ name +"';";
	if(!mysql_query(&con, str.c_str())){//为0成功
		//std::cout << "2" <<std::endl;
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
	std::string str = "insert into user values('"+ name +"','"+ score +"');";
	std::cout << str << std::endl;
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
