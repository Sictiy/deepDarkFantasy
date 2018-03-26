#include "dbhandler.h"
#include "client.h"
#include "server.h"

DbHandler::DbHandler(){

}

DbHandler::~DbHandler(){

}

void DbHandler::handleMsg(Msg *msg){
	std::cout << msg->fd << "--" << msg->cmd << std::endl;
	switch(msg->cmd){
		case d2h_get:
			getRank(msg);
			break;
		case d2h_insert:
			break;
		default:
			break;
	}
}

void DbHandler::getRank(Msg *msg){
	std::vector<RoleData> v;
	deepdf::DataResp *resp = new deepdf::DataResp();
	resp->ParseFromArray(msg->buff,strlen(msg->buff));
    for(int i=0;i<(resp->users_size());i++){
        deepdf::UserInfo  role = resp->users(i);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
        RoleData roledata;
        roledata.name = role.name();
        roledata.score = role.score();
        v.push_back(roledata);
    }
    Iclient.setRank(v);
	delete resp;
}

void DbHandler::selectRank(){
	Msg msg;
	msg.cmd = h2d_get;
	msg.fd = server.getDbFd();
	DbHandler::sendMsg(&msg);
}

void DbHandler::sendDataToDb(int cmd, char *buff){
	Msg *msg;
	msg->cmd = h2d_insert;
	msg->fd = server.getDbFd();
	memcpy(msg->buff,buff,strlen(buff));
	DbHandler::sendMsg(msg);
}