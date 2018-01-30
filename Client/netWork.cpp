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
#include "Protos.pb.h"

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

int runInString(int socketfd){
    char buff[2048];
    bool looping = true;
    while(looping){
        bzero(buff,strlen(buff));
        std::cin >> buff;
        if(strlen(buff)==1){
            looping = false;
            close(socketfd);
            break;
        } 
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

int runInProto(int socketfd){
    bool looping = true;
    while(looping){
        deepdf::UserInfo *role = new deepdf::UserInfo();
        //std::string name;
        //std::cin >> name;
        role->set_name("xlm");
        //int score;
        //std::cin >> score;
        role->set_score(100);
        char *buff = new char[role->ByteSize()];
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

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    int socketfd = connect("111.230.247.17",5050);
    //int socketfd = connect("127.0.0.1",5050);
    if(socketfd >= 0){
        std::cout << "successfully connect to server! fd:" << socketfd << std::endl;
        runInProto(socketfd);
    }
    close(socketfd);

    google::protobuf::ShutdownProtobufLibrary();
}
