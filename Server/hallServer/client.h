#ifndef __CLIENT__
#define __CLIENT__

#include <string>

class Client 
{
public:
	Client(int fd);
	virtual ~CLient();

	void disconnected();
	char* getBuff();
	int process();

private:
	void sendData(char* data);

private:
	char Buff[2048];
	int Fd;
};

#endif