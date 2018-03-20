#ifndef __CLIENT__
#define __CLIENT__

#include "head.h"
#include "Protos.pb.h"

class Client 
{
public:
	Client();
	virtual ~Client();

	void pushRespond(const std::vector<RoleData>& vec);
	void pushRequest(ClientRequest request);
	void run();
	bool init();

private:
	void sendData(int fd,int cmd_id ,char* data,int length);
	void selectRole();
	void createRole(const ClientRequest& request);
	void insertRole(const RoleData& role);
	void formatData(int fd);
	void getDataFromDB();
	void process();
	void processOneRequest(const ClientRequest& request);
	void pushCmd(Cmd cmd);

private:
	int MinScore;
	bool GetDbData;
	std::thread* Thread;
	std::vector<RoleData> RoleDataList;
	std::deque<ClientRequest> Requests;

	std::deque<Cmd> cmds;
};

#endif