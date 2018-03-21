#include "server.h"
#include "client.h"
#include "network.h"

Network::Network():
	Epoolfd(0),
	Listenfd(0),
	Thread(nullptr),
	Frame(0),
	Isloop(true){
	ClientMap.clear();
}

Network::~Network(){
	Isloop = false;
	if(nullptr != Thread){
		Thread->join();
	}
	delete Thread;
	close(Listenfd);

	ClientMap.clear();
	delete ClientMgr;
	std::cout << "close server!"<<std::endl;
}

bool Network::createServer(const char* ip, int port){
	unsigned long m_addr = INADDR_ANY;

	Listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Listenfd == -1){
		std::cout << "create socket failed"<<std::endl;
	}

	if (ip[0] != '\0')
    {
        m_addr = inet_addr(ip);
        if (m_addr == INADDR_NONE)
            m_addr = INADDR_ANY;
    }

	struct sockaddr_in server_addr;
	memset(&server_addr, 0 , sizeof(sockaddr_in));  //set zero
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = m_addr;
	server_addr.sin_port = htons(port);

	if(-1 == bind(Listenfd, (struct sockaddr*)(&server_addr), sizeof(server_addr))){
		std::cout << " bind socket failed"<<std::endl;
		return false;
	}

	if(-1 == listen(Listenfd, 8)){
		std::cout << " listen socket failed"<<std::endl;
		return false;
	}

	Epoolfd = epoll_create(MAXEVENTSIZE);
	ctlEvent(Listenfd, true);
	std::cout << " CreateServer:"<<ip<<":"<<port<<" Success!"<<std::endl;

	return true;
}

void Network::ctlEvent(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(Epoolfd, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		setNoblock(fd);
		Cli newcli;
		newcli.fd = fd;
		newcli.offset = 0;
		newcli.length = MAXLEN;
		//memset(&newcli,0,sizeof(Cli));
		ClientMap[fd] = newcli;
		if(nullptr == ClientMgr){
			std::cout << "init ClientMgr"<< std::endl;
			ClientMgr = new Client();
			ClientMgr->run();
		}
		if(fd != Listenfd)
			std::cout << fd << " add to loop." << std::endl;
	}
	else{
		close(fd);
		//delete ClientMap[fd];
		ClientMap.erase(fd);
		std::cout << fd << " exit from loop." << std::endl;
	}
}

int Network::epollLoop(){
	using namespace std::chrono;
    steady_clock::time_point tpBegin = steady_clock::now();

	struct sockaddr_in client_addr;
	socklen_t client_len;
	int nfds = 0;
	int fd = 0;
	int bufflen = 0;
	struct epoll_event events[MAXEVENTSIZE];
	while(true){
		nfds = epoll_wait(Epoolfd, events, MAXEVENTSIZE, TIMEWAIT);
		for(int i = 0; i < nfds; i++){
			std::cout << "fd:+++++" << events[i].data.fd<< std::endl;
			if(events[i].data.fd == Listenfd){
				fd = accept(Listenfd, (struct sockaddr*)&client_addr, &client_len);
				ctlEvent(fd, true);
				char ipAddress[INET_ADDRSTRLEN];
				std::cout << "accept connection from :"<<inet_ntop(AF_INET, &client_addr.sin_addr, ipAddress, sizeof(ipAddress))<<":"<<ntohs(client_addr.sin_port)<< std::endl;
				/*********************************/
				ClientRequest request;
				request.fd = fd;
				request.cmd_id = Select;
				memset(request.buff, 0, (size_t)2048);
				ClientMgr->pushRequest(request);
			}
			else if(events[i].events & EPOLLIN){
				if((fd == events[i].data.fd) < 0) 
					continue;
				processTcpPackage(events[i].data.fd);
			}
		}

		steady_clock::time_point tpNow = steady_clock::now();
        milliseconds dur = duration_cast<milliseconds>(tpNow - tpBegin);

        if (dur.count() * 30 < Frame * 1000)
        {
            std::this_thread::sleep_for(milliseconds(100));
            continue;
        }

        Frame++;
	}
}

void Network::setCliMgr(Client* client){
	ClientMgr = client;
}

int Network::setNoblock(int fd){
	int flags;
	if((flags == fcntl(fd, F_GETFL, 0)) == -1) 
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Network::run(){
	std::cout << "network is runing!" <<std::endl;
	Thread = new std::thread(&Network::epollLoop, this);
	//Thread->detach();
}

void Network::processTcpPackage(int fd){
	std::cout << "get data from:" << fd << std::endl;
	Cli cli = ClientMap[fd];
	int readlen = read(fd, cli.buff+cli.offset, MAXLEN-cli.offset);
	cli.offset += readlen;
	if(cli.offset <= 0){
		ctlEvent(fd, false);
		return ;
	}

	if(cli.offset >= 2){
		//memcpy(&cli->length,cli->buff,2);
		cli.length = cli.buff[0]+(cli.buff[1]<<8);
		//std::cout << "new length: " << cli.length << std::endl;
	}
	//std::cout << "new offset: " << cli.offset << std::endl;
	
	if(cli.offset >= cli.length){ //get all request data
		//std::cout << "get all request data" << std::endl;
		short length = cli.length;
		ClientRequest request;
		request.fd = fd;

		short cmd = cli.buff[2]+(cli.buff[3]<<8);
		std::cout << "cmd:" << cmd << std::endl;
		request.cmd_id = Insert;
		//memset(request.buff ,0 ,(size_t)MAXLEN);
		//std::cout <<"serialize length: "<< cli.length<<std::endl;
		memcpy(request.buff,cli.buff+4,length-4);
		std::cout <<"bufflen:"<< strlen(request.buff)<<std::endl;
		ClientMgr->pushRequest(request);

		//std::cout << "reset cli buff" << std::endl;
		cli.offset -= cli.length;
		if(cli.offset > 0){
			memcpy(cli.buff,cli.buff+cli.length,cli.offset);
		}
		cli.length = MAXLEN;
		//std::cout <<"new length: "<< cli.length<<std::endl;
	}
}