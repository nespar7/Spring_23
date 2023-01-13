#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <poll.h>
#include <time.h>

int main(){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int response;
    char buff[50];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("Socket created\n");

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(20000);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(response < 0){
        perror("Unable to bind");
        close(sockfd);
        exit(0);
    }

    printf("Server running...\n");

    while(1){
        socklen_t clilen = sizeof(cliaddr);
        response = recvfrom(sockfd, (char *)buff, 50, 0, (struct sockaddr*)&cliaddr, &clilen);
        if(response < 0){
            perror("Could not receive datagram");
            exit(0);
        }
        printf("Client sent: %s\n", buff);

        // Storing server time in buff
        time_t t;
        t = time(NULL);
        snprintf(buff, sizeof(buff), "%s", ctime(&t));

        response = sendto(sockfd, (const char *)buff, strlen(buff), 0, (struct sockaddr*)&cliaddr, clilen);
        if(response < 0){
            perror("Could not send data");
            exit(0);
        }
    }

    close(sockfd);
    return 0;
}