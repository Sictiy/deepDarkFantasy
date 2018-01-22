#include "server.h"

using namespace std;
int main(){
	Server server;
	if(server.createServer("0.0.0.0",SERVERPORT)){
		server.run();
		delete &server;
	}
}