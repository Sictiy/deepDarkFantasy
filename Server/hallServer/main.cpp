#include "server.h"
#include <fstream>

using namespace std;
int main(int argc, char* argv[]){

	/*ofstream of("log.txt");
	streambuf* fileBuf = of.rdbuf();
	cout.rdbuf(fileBuf);*/
	FLAGS_log_dir = "./"; 
	google::InitGoogleLogging(argv[0]);

	Server server;
	if(server.init()){
		server.run();
		delete &server;
	}

	google::ShutdownGoogleLogging();
	/*of.flush();
	of.close();*/
	return 0;
}