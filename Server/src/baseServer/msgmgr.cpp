#include "msgmgr.h"

MsgMgr::MsgMgr(){
	
}

MsgMgr::~MsgMgr(){

}

void MsgMgr::init(){
	msgs = new MsgQueue("msgs");
	run();
}

void MsgMgr::run(){
	std::cout << "msgmgr is runing!"<<std::endl;	
	std::thread* Thread = new std::thread(&MsgMgr::process,this);
}

void MsgMgr::process(){
	using namespace std::chrono;
	while(true){
		steady_clock::time_point tpBegin = steady_clock::now();
		Msg *msg = msgs->recvMsg();
		if(msg!=nullptr){
			std::cout << "get a new msg! cmd:" <<msg->cmd<< std::endl;
			dispatchMsg(msg);
		}
		milliseconds dur;
        do{
			steady_clock::time_point tpNow = steady_clock::now();
	        dur = duration_cast<milliseconds>(tpNow - tpBegin);
            std::this_thread::sleep_for(milliseconds(10));
        }while (dur.count() * FPS < 1000);
	}
}

void MsgMgr::addMsg(Msg *msg){
	msgs->sendMsg(msg);
}

void MsgMgr::addHandler(int cmd,Handler *handler){
  	if (handler == NULL)
    	return;
	auto pair = handler_map_.insert(std::make_pair(cmd, HandlerList()));
	auto it = pair.first;
	if (it == handler_map_.end())
		return;
	it->second.push_back(handler);
}

void MsgMgr::removeHandler(int cmd,Handler *handler){
	if (handler == NULL)
		return;
	auto it = handler_map_.find(cmd);
	if (it == handler_map_.end())
		return;
	auto listIt = std::find(it->second.begin(), it->second.end(), handler);
	if (listIt != it->second.end())
		it->second.erase(listIt);
}

void MsgMgr::clearHandler(int cmd){
	handler_map_.erase(cmd);
}

void MsgMgr::dispatchMsg(Msg *msg){
	auto it = handler_map_.find(msg->cmd);
	if (it == handler_map_.end())
		return;
	auto &handler_list = it->second;
	for (auto listIt = handler_list.begin();listIt != handler_list.end();listIt++){
		if ((*listIt)==NULL)
			continue;
		(*listIt)->handleMsg(msg);
	}
}