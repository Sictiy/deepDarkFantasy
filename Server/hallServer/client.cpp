#include "client.h"

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
	createRequest();
	getDataFromDb();
	processDBData();
}

void Client::createRole(){
	std::vector<std::string> str;
	if(USESTRING){
		std::cout << "get request!" << std::endl;
		splitstr(Buff,",",str);
		RoleData newrole;
		strcpy(newrole.name,str[0].c_str());
		if(str.size()==1)
			newrole.score = 0;
		else
			newrole.score = atoi(str[1].c_str());
		std::cout << "role data:!" <<newrole.name<<","<<newrole.score<< std::endl;
		RoleDataList.push_back(newrole);
	}else{
		std::cout << "use other data!"<< std::endl;
	}
	std::cout << "add data to vector!"<<std::endl;
}

void Client::selectRole(){
	
}

void Client::getDataFromDb(){
	
}

void Client::processDBData(){
	sort(RoleDataList.begin(),RoleDataList.end(),compare);
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
}

void Client::sendData(std::string data){
	write(Fd, data.c_str(), data.length());
	std::cout << "send data success !data:\n" <<data<< std::endl;
}

char* Client::getBuff(){
	return Buff;
}