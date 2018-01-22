#include "server.h"

int main(int argc, char* argv[])
{
	if (!initSever())
	{
		exitServer();
    	return SERVER_SETUP_FAIL;
	}

	if (!startService())
	{
	    showServerStartMsg(FALSE);
		exitServer();
    	return SERVER_SETUP_FAIL;
	}

    inputAndOutput();

	exitServer();

	return 0;
}
