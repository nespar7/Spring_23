#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 20000
#define BUFFSIZE 100

int main()
{
    int sockfd;
    char buff[BUFFSIZE];
    char *message;
    struct sockaddr_in servaddr, cliaddr;
    int response;

    // Creating a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Created socket!\n");

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Binding the socket to local address, port 20000
    response = bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (response < 0)
    {
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        socklen_t len;
        len = sizeof(cliaddr);

        // Receiving a datagram from the client
        response = recvfrom(sockfd, (char *)buff, BUFFSIZE, 0, (struct sockaddr *)&cliaddr, &len);
        if (response < 0)
        {
            perror("Could not receive");
            exit(EXIT_FAILURE);
        }
        buff[response] = '\0';
        printf("Received from client: %s\n", buff);

        // Storing the server time in message
        time_t t;
        t = time(NULL);
        message = ctime(&t);

        // Sending the time to client
        response = sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *)&cliaddr, len);
    }
    
    close(sockfd);

    return 0;
}