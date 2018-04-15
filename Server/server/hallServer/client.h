#ifndef __CLIENT__
#define __CLIENT__

#include "clihandler.h"
#include "dbhandler.h"
#include "../baseServer/head.h"
#include "../baseServer/Protos.pb.h"
#include "../baseServer/msgqueue.h"

class Client 
{
public:
	Client();
	static Client &Instance(){
		static Client inst_;
		return inst_;
	}
	virtual ~Client();

	void getRank(Msg *msg);
	void insert(Msg *msg);
	void setRank(const std::vector<RoleData> &v);

	// void pushRespond(const std::vector<RoleData>& vec);
	// void pushRequest(ClientRequest request);
	// void setMsgQueue(MsgQueue *m);
	// void setCmdDeque(std::deque<Cmd> &cmds_in);
	//void run();
	//bool init();

private:
	// void sendData(int fd,int cmd_id ,char* data,int length);
	// void selectRole();
	// void createRole(const ClientRequest& request);
	void insertRole(const RoleData& role);
	void formatData(int fd);
	// void getDataFromDB();
	// void process();
	// void processOneRequest(const ClientRequest& request);
	//void pushCmd(Cmd cmd);

private:
	int MinScore;
	// bool GetDbData;
	// std::thread* Thread;
	std::vector<RoleData> RoleDataList;
	// std::deque<ClientRequest> Requests;

	// MsgQueue* msgs;
};

#define Iclient Client::Instance()

#endif