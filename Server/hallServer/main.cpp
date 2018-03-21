#include "server.h"
#include "dbmgr.h"
#include "client.h"

using namespace std;
// std::deque<Cmd> Dbmgr::cmds;
int main(int argc, char* argv[]){
	using namespace std::chrono;
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	ofstream of("log.txt");
	streambuf* fileBuf = of.rdbuf();
	cout.rdbuf(fileBuf);

	Server *server = new Server();
	server->init();
	
	while(true){
		std::this_thread::sleep_for(milliseconds(2000));
	}

	of.flush();
	of.close();

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
