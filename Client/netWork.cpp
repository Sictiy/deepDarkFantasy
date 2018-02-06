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
        std::string name;
        std::cin >> name;
        int score;
        std::cin >> score;
        role->set_score(score);
        role->set_name(name);
        std::cout << "len of userinfo:" << role->ByteSize() << std::endl;
        char *buff = new char[role->ByteSize()];
        role->SerializeToArray(buff,role->ByteSize());

        short length = role->ByteSize()+4;
        //short len = ntohs(length);
        char * datatoh = new char[length];
        short cmd = 1;
        memcpy(datatoh,&length,2);
        memcpy(datatoh+2,&cmd,2);
        memcpy(datatoh+4,buff,length-4);

        int n = send(socketfd,datatoh,role->ByteSize()+4,0);
        std::cout <<"send:"<<"+++++"<<" len:"<<length<<"--"<< std::endl;

        deepdf::UserInfo *role2 = new deepdf::UserInfo();
        role2->ParseFromArray(buff,role->ByteSize());
        std::cout << role2->name() << "--"<<role2->score()<< std::endl;
        char*  newbuff = new char[1024*64];
        bzero(newbuff,strlen(newbuff));
        recv(socketfd,newbuff,1024*64,0);
        //    sleep(1000);
        short recvlen=0;
        memcpy(&recvlen,newbuff,2);
        //short recvlength = htons(recvlen);
        std::cout <<"recv:len:"<< recvlen<<"-++---"<< std::endl;
        recvlen = newbuff[2]+(newbuff[3]<<8);
        std::cout <<"recv:len:"<< recvlen<<"----++-"<< std::endl;



        deepdf::DataResp * resp = new deepdf::DataResp();
        resp->ParseFromArray(buff+4,strlen(buff)-4);
        for(int i=0;i<(resp->users_size());i++){
            deepdf::UserInfo  role = resp->users(i);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
            std::cout << role.name()<<"--" <<role.score()<< std::endl;
        }
        /*while( recv(socketfd,buff,2048,0) <=0 ){
            sleep(1000);            
        }
        std::cout <<"++++++++++++++++++++++++"<< std::endl;
        deepdf::UserInfo *role3 = new deepdf::UserInfo();
        role3->ParseFromArray(buff,role->ByteSize());
        std::cout << role3->name() << "--"<<role3->score()<< std::endl;*/

    }
    return 0;
}
int main(){

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    //int socketfd = connect("111.230.247.17",5050);
    int socketfd = connect("127.0.0.1",5050);
    if(socketfd >= 0){
        std::cout << "successfully connect to server! fd:" << socketfd << std::endl;
        runInProto(socketfd);
    }
    close(socketfd);
    google::protobuf::ShutdownProtobufLibrary();
}
