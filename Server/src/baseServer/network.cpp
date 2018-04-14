//#include "server.h"
#include "network.h"

Network::Network():
	Epoolfd(0),
	Listenfd(0),
	Thread(nullptr),
	Frame(0){
	PacketMap.clear();
}

Network::~Network(){
	if(nullptr != Thread){
		Thread->join();
	}
	delete Thread;
	close(Listenfd);

	PacketMap.clear();
	std::cout << "close network!"<<std::endl;
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
	std::cout << "CreateListenPort:"<<ip<<":"<<port<<" Success!"<<std::endl;

	return true;
}

int Network::connectServer(const char* ip, int port){
	int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1){
        std::cout << "new socket failed!\n" << std::endl;
        return -1;
    }

    struct sockaddr_in sa;
    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(ip);
    bzero(&(sa.sin_zero), 8);

    if(-1 == connect(socketfd, (struct sockaddr *)&sa, sizeof(sa))){
        printf("connect failed!\n");
        return -1;
    }
    ctlEvent(socketfd,true);
    return socketfd;
}

void Network::ctlEvent(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(Epoolfd, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		setNoblock(fd);

		Packet* packet = new Packet(fd);
		PacketMap[fd] = packet;
		
		if(fd != Listenfd)
			std::cout <<"fd:"<< fd << " add to loop." << std::endl;
	}
	else{
		close(fd);
		//delete ClientMap[fd];
		PacketMap.erase(fd);
		std::cout <<"fd:"<< fd << " exit from loop." << std::endl;
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
			std::cout << "get msg event form fd:" << events[i].data.fd<< std::endl;
			if(events[i].data.fd == Listenfd){
				fd = accept(Listenfd, (struct sockaddr*)&client_addr, &client_len);
				ctlEvent(fd, true);
				char ipAddress[INET_ADDRSTRLEN];
				std::cout << "accept connection from :"<<inet_ntop(AF_INET, &client_addr.sin_addr, ipAddress, sizeof(ipAddress))<<":"<<ntohs(client_addr.sin_port)<< std::endl;

				Msg * msg = new Msg;
				msg->fd = fd;
				msg->cmd = fd_newconnect;
				memset(msg->buff,0,(size_t)2048);
				msgMgr.addMsg(msg);     
			}
			else if(events[i].events & EPOLLIN){
				if((fd == events[i].data.fd) < 0) 
					continue;
				processTcpPackage(events[i].data.fd);
			}
		}

		steady_clock::time_point tpNow = steady_clock::now();
        milliseconds dur = duration_cast<milliseconds>(tpNow - tpBegin);

        if (dur.count() * 1 < Frame * 1000)
        {
            std::this_thread::sleep_for(milliseconds(100));
            continue;
        }

        Frame++;
	}
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
	Thread->detach();
}

void Network::processTcpPackage(int fd){
	std::cout << "get data from:" << fd << std::endl;
	Packet *packet = PacketMap[fd];
	switch(packet->addPacket(fd)){
		case -1:
			ctlEvent(fd,false);
			break;
		case 1:
			Msg msg ;
			msg = packet->recvMsg();
			msgMgr.addMsg(&msg);
			break;
		case 0:
			std::cout << "wait a new packet" << std::endl;	
			break;
	}
}
