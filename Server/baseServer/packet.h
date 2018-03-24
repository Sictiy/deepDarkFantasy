#ifndef __PACKET__
#define __PACKET__

#include "head.h"

class Packet
{
public:
	Packet(int);
	virtual ~Packet();

	int addPacket(int fd);
	short getLength();
	char* getBuff();
	int getFd();
	Cmd_id getCmd();
	short getType();

private:
	int fd;
	Cmd_id cmd;
	short type;
	char buff[MAXLEN];
	char cache[MAXLEN];

	short length;
	short offset;
	/* data */
};

#endif