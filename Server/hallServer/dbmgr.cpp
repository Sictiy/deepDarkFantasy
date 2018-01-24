#include "dbmgr.h"

Dbmgr::Dbmgr(){

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
	return true;
}

bool Dbmgr::run(){
	std::cout << "dbserver is runing!" << std::endl;
	Thread = new std::thread(&Dbmgr::process,this);
	//Thread->detach();
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
			processRequest(cmd);
		}
		std::this_thread::sleep_for(milliseconds(100));
	}
}

void Dbmgr::processRequest(Cmd cmd){
	//process cmd;


	//need to change
	RoleData role = cmd.data;
	RoleDataList.push_back(role);
	respond(cmd);
}

void Dbmgr::respond(Cmd cmd){
	cmd.client->pushRespond(RoleDataList);
}