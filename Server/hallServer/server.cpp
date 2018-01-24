#include "server.h"
#include "client.h"

/*******************************************/
bool loop = true;
void sig_handler( int sig )
{
    if ( sig == SIGINT ||sig == SIGQUIT||sig == SIGTERM)
    {
        loop = false;
    }
}
/*******************************************/

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
	close(Listenfd);

	for(auto iterator:ClientMap){
		close(iterator.first);
		delete iterator.second;
	}

	ClientMap.clear();
	std::cout << "close server!"<<std::endl;
}

bool Server::init(){
	if(createServer("0.0.0.0",SERVERPORT)){
		setLog();
		setDaemon();
		return true;
	}
	return false;
}

bool Server::createServer(const char* ip, int port){
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

void Server::ctlEvent(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(Epoolfd, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		setNoblock(fd);
		ClientMap[fd] = new Client(fd);
		if(fd != Listenfd)
			std::cout << fd << " add to loop." << std::endl;
	}
	else{
		close(fd);
		delete ClientMap[fd];
		ClientMap.erase(fd);
		std::cout << fd << "exit from loop." << std::endl;
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
		nfds = epoll_wait(Epoolfd, events, MAXEVENTSIZE, TIMEWAIT);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == Listenfd){
				fd = accept(Listenfd, (struct sockaddr*)&client_addr, &client_len);
				ctlEvent(fd, true);
				char ipAddress[INET_ADDRSTRLEN];
				std::cout << "accept connection from :"<<inet_ntop(AF_INET, &client_addr.sin_addr, ipAddress, sizeof(ipAddress))<<":"<<ntohs(client_addr.sin_port)<< std::endl;
			}
			else if(events[i].events & EPOLLIN){
				if((fd == events[i].data.fd) < 0) 
					continue;

				Client* client = ClientMap[fd];
				if(NULL == client) 
					continue;

				memset(client->getBuff(), 0, (size_t)1024);
				if((bufflen = read(fd, client->getBuff(), BUFFLEN)) <= 1 ) {
					ctlEvent(fd, false);
					client->disconnected();
				}
				else{
					std::cout <<"bufflen:"<< bufflen <<std::endl;
					std::thread * subthread = new std::thread(&Client::process, client);
					subthread->detach();
				}
			}
		}

		steady_clock::time_point tpNow = steady_clock::now();
        milliseconds dur = duration_cast<milliseconds>(tpNow - tpBegin);

        if (dur.count() * 30 < Frame * 1000)
        {
            std::this_thread::sleep_for(milliseconds(8));
            continue;
        }

        Frame++;
	}
}

int Server::setNoblock(int fd){
	int flags;
	if((flags == fcntl(fd, F_GETFL, 0)) == -1) 
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::run(){
	std::cout << "server is runing!" <<std::endl;
	Thread = new std::thread(&Server::epollLoop, this);
	//Thread->detach();
}

void Server::setLog(){
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
}

void Server::setDaemon(){
	if(-1==daemon(0,0)){
		std::cout << "daemon erro!" << std::endl;
		exit(1);
	}
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
}