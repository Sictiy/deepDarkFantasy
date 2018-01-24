#ifndef __CLIENT__
#define __CLIENT__

#include "head.h"

class Client 
{
public:
	Client(int fd);
	virtual ~Client();

	void disconnected();
	char* getBuff();
	void process();
	void pushRespond(const std::vector<RoleData>& vec);

private:
	void sendData(std::string data);
	void selectRole();
	void createRole();
	void insertRole(const RoleData& role);
	void getDataFromDB();
	void processDBData();

private:
	char Buff[2048];
	int Fd;
	int Listlen;
	std::vector<RoleData> RoleDataList;
};

#endif