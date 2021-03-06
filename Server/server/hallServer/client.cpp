#include "client.h"
//#include "../baseServer/dbmgr.h"

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
Client::Client(){
	// RoleDataList.clear();
}

Client::~Client(){
}

void Client::getRank(Msg *msg){
	formatData(msg->fd);
}

void Client::setRank(const std::vector<RoleData>& vec){  //
	RoleDataList.assign(vec.begin(),vec.end());
	sort(RoleDataList.begin(),RoleDataList.end(),compare);
	for(int i=0;i<RoleDataList.size();i++){
		RoleData roledata = RoleDataList.at(i);
		std::cout << roledata.name << "+++" << roledata.score << std::endl;
	}
	std::cout << "load rank data successful!" <<std::endl;
}

void Client::insert(Msg *msg){
	RoleData newrole;
	deepdf::UserInfo *role = new deepdf::UserInfo() ;
	role->ParseFromArray(msg->buff,strlen(msg->buff));
	std::cout <<"new role:"<< role->name()<<"--"<<role->score()<<std::endl;
	newrole.score = role->score();
	newrole.name = role->name();
	delete role;

	if(RoleDataList.size()>=10){
		MinScore = RoleDataList.at(9).score;
	}else if(RoleDataList.size()==0){
		MinScore = 0;
	}else{
		MinScore = RoleDataList.back().score;
	}
	std::cout << "min-score, new-score:"<<MinScore <<"--"<< newrole.score << std::endl;
	if((RoleDataList.size() < 10 || MinScore <= newrole.score) && newrole.score>100 && newrole.score<200000 &&newrole.name.length() < 20)
		insertRole(newrole);
    formatData(msg->fd);
}

void Client::formatData(int fd){
	deepdf::DataResp *respond = new deepdf::DataResp();
	int len;
	if(RoleDataList.size()<=10)
		len = RoleDataList.size();
	else
		len = 10;
	for(int i=0;i<len;i++){
		RoleData roledata = RoleDataList.at(i);
		deepdf::UserInfo *role = respond->add_users();
		role->set_name(roledata.name);
		role->set_score(roledata.score); 
	}
	int length = respond->ByteSize();
	char* data = new char[length];
	respond->SerializeToArray(data,length);

	CliHandler::sendData(fd,s2c_rank_get,data);
}

void Client::insertRole(const RoleData& role){
	std::cout << "client:insert role!" << std::endl;
	deepdf::UserInfo *info = new deepdf::UserInfo();
	info->set_score(role.score);
	info->set_name(role.name);
	char* data = new char[info->ByteSize()];
	info->SerializeToArray(data,info->ByteSize());

	DbHandler::sendDataToDb(h2d_insert,data);

	RoleDataList.push_back(role);
	sort(RoleDataList.begin(),RoleDataList.end(),compare);
}

// /***********************************************************************************************/
// bool Client::init(){
// 	selectRole();
// 	//getDataFromDB();
// 	return true;
// }

// void Client::processOneRequest(const ClientRequest& request){
// 	//std::cout<<"BUFF:"<<request.buff<< std::endl;
// 	if(request.cmd_id == Insert){
// 		createRole(request);
// 	}
// 	formatData(request.fd);
// }

// void Client::createRole(const ClientRequest& request){
// 	const char *Buff = request.buff;
// 	RoleData newrole;
// 	if(USESTRING){
// 		//std::cout << "get request by string!" << std::endl;
// 		std::vector<std::string> str;
// 		splitstr(Buff,",",str);
// 		newrole.name = str[0];
// 		//strcpy(newrole.name,str[0].c_str());
// 		if(str.size()==1)
// 			newrole.score = 0;
// 		else
// 			newrole.score = atoi(str[1].c_str());
// 		//std::cout << "role data:!" <<newrole.name<<","<<newrole.score<< std::endl;
// 		//RoleDataList.push_back(newrole);
// 	}else{
// 		std::cout << "get request by protobuf!"<< std::endl;

// 		//std::cout <<"recv:len:"<< strlen(Buff)<<"-++-"<<Buff << std::endl;
//         deepdf::DataResp * resp = new deepdf::DataResp();
//         resp->ParseFromArray(Buff,strlen(Buff));
//         for(int i=0;i<(resp->users_size());i++){
//             deepdf::UserInfo  role = resp->users(i);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
//             std::cout << role.name()<<"--" <<role.score()<< std::endl;
//         }
// 		delete resp;
// 		std::cout << "+++++++++++++++++++++++++++++++++++"<< std::endl;

// 		deepdf::UserInfo *role = new deepdf::UserInfo() ;
// 		role->ParseFromArray(Buff,strlen(Buff));
// 		std::cout << role->name()<<"--"<<role->score()<<std::endl;
// 		newrole.score = role->score();
// 		newrole.name = role->name();
// 		delete role;
// 	}
// 	//std::cout << "add data to vector!"<<std::endl;
// 	if(RoleDataList.size()>=10){
// 		MinScore = RoleDataList.at(9).score;
// 	}else if(RoleDataList.size()==0){
// 		MinScore = 0;
// 	}else{
// 		MinScore = RoleDataList.back().score;
// 	}
// 	std::cout << "min-score, new-score:"<<MinScore <<"--"<< newrole.score << std::endl;
// 	if((RoleDataList.size() < 10 || MinScore <= newrole.score) && newrole.score>100 && newrole.score<200000 &&newrole.name.length() < 20)
// 		insertRole(newrole);
// }

// void Client::sendData(int fd,int cmd_id, char * data, int length){
// 	//add length to data
// 	length += 4;
// 	std::cout << "length:" << length<< std::endl;
// 	char * datatoh = new char[length];
//     bzero(datatoh,strlen(datatoh));
// 	memcpy(datatoh,&length,2);
// 	memcpy(datatoh+2,&cmd_id,2);
// 	memcpy(datatoh+4,data,length-4);
// 	int sendlen = 0;
// 	while(sendlen < length){
// 	//	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" <<std::endl;
// 		sendlen += write(fd, datatoh+sendlen, length-sendlen);
// 	}
// 	//std::cout << "send data success !data:\n" <<data<< std::endl;

// 	//short ll = datatoh[0]+(datatoh[1]<<8);
// 	//std::cout << "serialize length:"<<"++++"<<ll<<std::endl;
// }
// /***************************************/
// void Client::selectRole(){
// 	std::cout << "client:selectRole" <<std::endl;
// 	Cmd cmd;
// 	cmd.client = this;
// 	cmd.cmd_id = Select;
// 	//Dbmgr::pushRequest(cmd);
// 	msgs->sendMsg(cmd);
// }


// // void Client::pushCmd(Cmd cmd){
// // 	cmds.push_front(cmd);
// // }

// void Client::getDataFromDB(){
// 	std::cout << "client:get data from db 1" <<std::endl;
// 	using namespace std::chrono;
// 	while(!GetDbData){
// 		std::this_thread::sleep_for(milliseconds(100));
// 	}
// 	if(RoleDataList.size()>=10){
// 		MinScore = RoleDataList.at(9).score;
// 	}else{
// 		MinScore = RoleDataList.back().score;
// 	}
// 	std::cout << "client:get data from db 2" <<std::endl;
// }
// /***************************************/
// /*
// char* Client::getBuff(){
// 	return Buff;
// }
// /**************************************/
// void Client::process(){
// 	std::cout << "client: start process request loop!" <<std::endl;
// 	using namespace std::chrono;
// 	while(true){
// 		if(Requests.empty()){
// 			continue;
// 		}else{
// 			//std::cout << "client: start process request!" <<std::endl;
// 			processOneRequest(Requests.back());
// 			Requests.pop_back();
// 			continue;
// 		}
// 		std::this_thread::sleep_for(milliseconds(100));
// 	}
// }

// void Client::run(){
// 	std::cout << "clientmgr is runing!" <<std::endl;
// 	Thread = new std::thread(&Client::process,this);
// }

// void Client::pushRequest(ClientRequest request){
// 	Requests.push_front(request);
// 	std::cout << "client: get request!" <<std::endl;
// }

// // void Client::setCmdDeque(std::deque<Cmd> &cmds_in){
// // 	cmds = cmds_in;
// // }

// void Client::setMsgQueue(MsgQueue *m){
// 	msgs = m;
// }
