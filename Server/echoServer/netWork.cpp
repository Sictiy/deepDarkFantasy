#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

#define EHCO_PORT    8080
#define MAX_CLIENT_NUM        10

using namespace std;
int main()
{
    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
       
    if(socketfd == -1)
    {
        cout << "new socket failed!" << endl;
        return -1;
    }
    else
    {
        printf("socket create successfully ");
    }

    struct sockaddr_in sa;
    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(EHCO_PORT);
    sa.sin_addr.s_addr = htons(INADDR_ANY);
    bzero(&(sa.sin_zero), 8);

    if(bind(socketfd, (struct sockaddr *)&sa, sizeof(sa))!= 0)
    {
        printf("bind failed ");
        return -1;
    }
    else
    {
        printf("bind successfully ");
    }

    //listen
    if(listen(socketfd ,MAX_CLIENT_NUM) != 0)
    {
        printf("listen error ");
        return -1;
    }
    else
    {
        printf("listen successfully ");
    }

    int clientfd;
    struct sockaddr_in clientAdd;
    char buff[101];
    socklen_t len = sizeof(clientAdd);
    int closing =0;
    while( closing == 0  && (clientfd = accept(socketfd, (struct sockaddr *)&clientAdd, &len)) >0 )
    {
        int n;
        while((n = recv(clientfd,buff, 100,0 )) > 0)
        {
            printf("number of receive bytes = %d ", n);
            write(STDOUT_FILENO, buff, n);
            send(clientfd, buff, n, 0);
            buff[n] = '\0';
	    //memcpy(buff+n,' ',1);	    
            if(strcmp(buff, "quit") == 0)
            {
                break;
            }
            else if(strcmp(buff, "close") == 0)
            {
                //server closing
                closing = 1;
                printf("server is closing ");
                break;
            }
        }

        close(clientfd);
    }

    close(socketfd);

    return 0;
}
