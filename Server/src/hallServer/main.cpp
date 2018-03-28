#include "server.h"
#include "../baseServer/Protos.pb.h"

using namespace std;
int main(int argc, char* argv[]){
	using namespace std::chrono;
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	ofstream of("log_hall.txt");
	streambuf* fileBuf = of.rdbuf();
	cout.rdbuf(fileBuf);

	Server *server = new Server();
	
	if(server->init()){
		while(true){
			std::this_thread::sleep_for(milliseconds(2000));
		}
	}
	std::cout << "init server failed! " << std::endl;
	of.flush();
	of.close();

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
