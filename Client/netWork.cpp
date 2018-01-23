#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

using namespace std;
int connect(const char * host,int port){
    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd == -1){
        cout << "new socket failed!\n" << endl;
        return -1;
    }
    else{
        printf("socket create successfully \n");
    }

    struct sockaddr_in sa;
    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(host);
    bzero(&(sa.sin_zero), 8);

    if(-1 == connect(socketfd, (struct sockaddr *)&sa, sizeof(sa))){
        printf("connect failed!\n");
        return -1;
    }
    return socketfd;
}

int run(int socketfd){
    char buff[2048];
    bool looping = true;
    while(looping){
        std::cin >> buff;
        int n = send(socketfd,buff,strlen(buff),0);
        std::cout <<"send:"<<buff <<" len:"<<strlen(buff)<< std::endl;
        bzero(buff,strlen(buff));
        while( recv(socketfd,buff,2048,0) <=0 ){
            sleep(1000);            
        }
        std::cout <<"recv:"<< buff << std::endl;
    }
    return 0;
}

int main(){
    int socketfd = connect("111.230.247.17",8080);
    std::cout << "successfully connect to server! fd:" << socketfd << std::endl;
    if(socketfd >= 0){
        run(socketfd);
    }else{
        std::cout << "connect failed!"<<std::endl;
    }
    close(socketfd);
}