#include "client.h"
#include "dbmgr.h"

void splitstr(const std::string& s, const std::string seperator, std::vector<std::string>& vec){
	std::string::size_type pos1, pos2;
	pos2 = s.find(seperator);
	pos1 = 0;
	while(std::string::npos != pos2){
		vec.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + seperator.size();
		pos2 = s.find(seperator, pos1);
	}

	if(pos1 != s.length()){
		vec.push_back(s.substr(pos1));
	}
	std::cout << "split success!len:"<<vec.size()<<std::endl;
}

bool compare(const RoleData &a, const RoleData & b){
	return a.score >b.score;
}

/**************************************************************************/
Client::Client(int fd):
Fd(fd)
{

}

Client::~Client(){

}

void Client::disconnected(){
	close(Fd);
}

void Client::process(){
	std::cout<<"BUFF:"<<Buff << std::endl;
	createRole();
	getDataFromDB();
}

void Client::createRole(){
	RoleData newrole;
	if(USESTRING){
		std::cout << "get request by string!" << std::endl;
		std::vector<std::string> str;
		splitstr(Buff,",",str);
		newrole.name = str[0];
		//strcpy(newrole.name,str[0].c_str());
		if(str.size()==1)
			newrole.score = 0;
		else
			newrole.score = atoi(str[1].c_str());
		std::cout << "role data:!" <<newrole.name<<","<<newrole.score<< std::endl;
		//RoleDataList.push_back(newrole);
	}else{
		std::cout << "get request by protobuf!"<< std::endl;
		deepdf::UserInfo *role = new deepdf::UserInfo() ;
		role->ParseFromArray(Buff,strlen(Buff));
		std::cout << role->name()<<"--"<<role->score()<<std::endl;
		newrole.score = role->score();
		newrole.name = role->name();
	}
	//std::cout << "add data to vector!"<<std::endl;
	insertRole(newrole);
}

void Client::insertRole(const RoleData& role){
	Listlen = RoleDataList.size();

	Cmd cmd;
	cmd.client = this;
	cmd.cmd_id = Insert;
	cmd.data = role;
	Dbmgr::pushRequest(cmd);
}

void Client::selectRole(){

}

void Client::pushRespond(const std::vector<RoleData>& vec){
	RoleDataList.assign(vec.begin(),vec.end());
}

void Client::getDataFromDB(){
	using namespace std::chrono;
	while(true){
		if(RoleDataList.size() > Listlen){
			processDBData();
			break;
		}
		std::this_thread::sleep_for(milliseconds(100));
	}
}

void Client::processDBData(){
	sort(RoleDataList.begin(),RoleDataList.end(),compare);
	if(USESTRING){
		std::cout << "sort success!" << std::endl;
		std::string data=std::string("request:");
		int len;
		if (RoleDataList.size()<=10)
			len = RoleDataList.size();
		else
			len = 10;
		for(int i=0;i<len;i++){
			RoleData roledata = RoleDataList.at(i);
			data.append("\n").append(roledata.name);
			char score[100];
			sprintf(score,"%d",roledata.score);
			data.append(",").append(score);
		}
		std::cout << "get data success!listlen: " <<len<< std::endl;
		sendData(data);
	}else{
		std::cout << "sort success ! in protobuf" << std::endl;
		deepdf::DataResp *respond = new deepdf::DataResp();
		int len;
		if(RoleDataList.size()<=10)
			len = RoleDataList.size();
		else
			len = 10;
		std::cout << "len fo list:" << len << std::endl;
		for(int i=0;i<len;i++){
			std::cout << "index fo list:" << i<< std::endl;
			RoleData roledata = RoleDataList.at(i);
			deepdf::UserInfo *role = respond->add_users();
			role->set_name(roledata.name);
			role->set_score(roledata.score); 
		}
		std::cout << "to userinfo success:"  << std::endl;
		int length = respond->ByteSize();
		char* data = new char[length];
		respond->SerializeToArray(data,length);
		std::cout << "serialize success!" << std::endl;
		sendData(data);
	}
}

void Client::sendData(std::string data){
	write(Fd, data.c_str(), data.length());
	std::cout << "send data success !data:\n" <<data<< std::endl;
}

char* Client::getBuff(){
	return Buff;
}