#include "luamgr.h"

using namespace std;
int main(int argc, char const *argv[])
{
	luaMgr.init();
	luaMgr.start();
	luaMgr.disConnect(100);
	Msg msg;
	msg.fd = 1;
	msg.cmd = 2;
	luaMgr.recvData(&msg);
	luaMgr.clear();
	std::cout << "end" << std::endl;
	/* code */
	return 0;
}
