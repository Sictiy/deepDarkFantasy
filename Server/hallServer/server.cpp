#include "server.h"
#include "dbmgr.h"
#include "client.h"

Server::Server():
	Epoolfd(0),
	Listenfd(0),
	Thread(nullptr),
	Frame(0),
	Isloop(true){
	ClientMap.clear();
}

Server::~Server(){
	Isloop = false;
	if(nullptr != Thread){
		Thread->join();
	}
	delete Thread;

	for(auto iterator:ClientMap){
		close(iterator.first);
		delete iterator.second;
	}

	ClientMap.clear();
	//dispose db
}

bool Server::createServer(const char* ip, int port){
	unsigned long m_addr = INADDR_ANY;

	Listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(Listenfd == -1){
		std::cout << "create socket failed"<<endl;
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
		std::cout << " bind socket failed"<<endl;
		return false;
	}

	if(-1 == listen(Listenfd, 8)){
		std::cout << " listen socket failed"<<endl;
		return false;
	}

	Epoolfd = epoll_create(MAXEVENTSIZE);
	ctlEvent(listenfd_, true);
	std::cout << " CreateServer:"<<ip<<port<<"Success!"<<endl;

	return true;
}

void Server::ctlEvent(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(epollfd_, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		set_noblock(fd);
		ClientMap[fd] = new Client(fd);
		if(fd != Listenfd)
			std::cout << fd << "add to loop." << endl;
	}
	else{
		close(fd);
		delete ClientMap[fd];
		ClientMap.erase(fd);
		std::cout << fd << "exit from loop." << endl;
	}
}

int Server::epollLoop(){
	using namespace std::chrono;
    steady_clock::time_point tpBegin = steady_clock::now();

	struct sockaddr_in client_addr;
	socklen_t client_len;
	int nfds = 0;
	int fd = 0;
	int bufflen = 0;
	struct epoll_event events[MAXEVENTSIZE];
	while(true){
		nfds = epoll_wait(epollfd_, events, MAXEVENTSIZE, TIMEWAIT);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == Listenfd){
				fd = accept(Listenfd, (struct sockaddr*)&client_addr, &client_len);
				ctl_event(fd, true);
				char ipAddress[INET_ADDRSTRLEN];
				std::cout << "accept connection from :"<<inet_ntop(AF_INET, &client_addr.sin_addr, ipAddress, sizeof(ipAddress))<<":"<<ntohs(client_addr.sin_port))<< endl;
			}
			else if(events[i].events & EPOLLIN){
				if((fd == events[i].data.fd) < 0) 
					continue;

				Client* client = ClientMap[fd];
				if(NULL == client) 
					continue;

				memset(client->getBuff(), 0, (size_t)1024);
				if((bufflen = read(fd, client->getBuff(), BUFFLEN)) <= 0 ) {
					ctl_event(fd, false);
					client->disconnected();
				}
				else{
					client->process();
				}
			}
		}

		steady_clock::time_point tpNow = steady_clock::now();
        milliseconds dur = duration_cast<milliseconds>(tpNow - tpBegin);

        if (dur.count() * 30 < m_nFrame * 1000)
        {
            std::this_thread::sleep_for(milliseconds(8));
            continue;
        }

        m_nFrame++;
	}
}

int Server::serNoBlock(int fd){
	int flags;
	if((flags == fcntl(fd, F_GETFL, 0)) == -1) 
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::run(){
	thread_ = new std::thread(&Server::epoolLoop, this);
	thread_->detach();
}