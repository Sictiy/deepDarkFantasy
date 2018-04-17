#include "server.h"
#include "Protos.pb.h"

using namespace std;
int main(int argc, char* argv[]){
	using namespace std::chrono;
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	ofstream of("log.txt");
	streambuf* fileBuf = of.rdbuf();
	cout.rdbuf(fileBuf);
	
	Server* server = Server::Instance();
	// Server* server = new Server();

	if(server->init()){
		server->run();
	}
	std::cout << "server exited! " << std::endl;
	of.flush();
	of.close();

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}