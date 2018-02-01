#include "server.h"
#include "dbmgr.h"
#include "client.h"

using namespace std;
std::deque<Cmd> Dbmgr::cmds;
int main(int argc, char* argv[]){

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	ofstream of("log.txt");
	streambuf* fileBuf = of.rdbuf();
	cout.rdbuf(fileBuf);
	//FLAGS_log_dir = "$HOME/logs"; 
	//google::InitGoogleLogging(argv[0]);*/

	Client *ClientMgr = new Client();
	Server *server = new Server(ClientMgr);
	Dbmgr *dbmgr = new Dbmgr(); 
	server->init();
	server->run();
	dbmgr->init();
	dbmgr->run();
	ClientMgr->init();
	ClientMgr->run();
	std::cout << "run success!" <<std::endl;
	delete server;
	delete dbmgr;
	delete ClientMgr;
	
	/*using namespace std::chrono;
	while(true){
		std::this_thread::sleep_for(milliseconds(2000));
	}*/

	//google::ShutdownGoogleLogging();
	of.flush();
	of.close();
	//*/
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
