#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 20000
#define BUFFSIZE 50

char *users_file = "users.txt";

char *receive_string(int sockfd){
    char buff[BUFFSIZE];
    char *received_string;
    int received_size = 50;
    int len = 0;
    int response;

    received_string = (char *)malloc(sizeof(char)*received_size);

    while (1)
    {
        response = recv(sockfd, buff, 50, 0);

        if (response < 0)
        {
            perror("Cannot receive data");
            close(sockfd);
            exit(0);
        }

        while (len + response >= received_size)
        {
            received_size += 100;
        }

        received_string = realloc(received_string, received_size);

        strcat(received_string, buff);

        if (buff[response - 1] == '\0')
        {
            break;
        }
    }

    return received_string;
}

int main(){
    int sockfd, newsockfd;
    int clilen;
    struct sockaddr_in servaddr, cliaddr;
    int response;
    char buff[BUFFSIZE];

    // Creating the socket sockfd
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    // bind
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(response < 0){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(sockfd, 5);

    // Concurrent server
    while(1){
        clilen = sizeof(cliaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);

        if(newsockfd < 0){
            perror("Accept failed");
            close(newsockfd);
            exit(EXIT_FAILURE);
        }

        if(fork() == 0){
            close(sockfd);
            strcpy(buff, "LOGIN:");

            response = send(newsockfd, buff, strlen(buff)+1, 0);
            if(response < 0){
                perror("Send failed");
                exit(EXIT_FAILURE);
            }

            char *username = receive_string(newsockfd);
            printf("%s", username);

            close(newsockfd);
            exit(0);
        }
    }
}