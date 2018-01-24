#include "server.h"
#include "dbmgr.h"

using namespace std;
std::deque<Cmd> Dbmgr::cmds;
int main(int argc, char* argv[]){

	ofstream of("log.txt");
	streambuf* fileBuf = of.rdbuf();
	cout.rdbuf(fileBuf);
	//FLAGS_log_dir = "$HOME/logs"; 
	//google::InitGoogleLogging(argv[0]);

	Server server;
	if(server.init()){
		server.run();
		Dbmgr dbmgr;
		if(dbmgr.init()){
			dbmgr.run();
			delete &dbmgr;
		}
		/*while(true){
			sleep(1000);
		}*/
		delete &server;
	}

	//google::ShutdownGoogleLogging();
	of.flush();
	of.close();
	return 0;
}