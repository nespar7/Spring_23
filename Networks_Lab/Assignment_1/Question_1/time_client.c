#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 20000

int main(){
    int sockfd;
    int len;
    char buff[100];
    int response;

    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket!\n");
        exit(0);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    // inet_aton(INADDR_ANY, &addr.sin_addr);
    addr.sin_addr.s_addr	= INADDR_ANY;

    response = connect(sockfd, (struct sockaddr*) &addr, sizeof(addr));

    if(response < 0){
        perror("Connection to server failed!\n");
        close(sockfd);
        exit(0);
    }
    printf("Connected Successfully!\n");

    response = recv(sockfd, buff, sizeof(buff)+1, 0);
    if(response < 0){
        perror("Receive failed!\n");
        close(sockfd);
        exit(0);
    }

    printf("Server time: %s", buff);

    close(sockfd);
}