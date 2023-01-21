#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

#define PORT 20000
#define BUFFSIZE 100

int main()
{
    int sockfd;
    char buffer[BUFFSIZE];
    char *message = "Requesting server time";
    struct sockaddr_in servaddr;
    int response;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int len;
    len = sizeof(servaddr);

    sendto(sockfd, (const char *)message, strlen(message), 0, (struct sockaddr *)&servaddr, len);
    printf("Message sent: %s\n", message);

    // Set a timeout of 3s and iterate 5 times
    for (int i = 0; i < 5; i++)
    {
        struct pollfd setfd;

        setfd.fd = sockfd;
        setfd.events = POLLIN;

        response = poll(&setfd, 1, 3000);

        if (response < 0)
        {
            perror("Poll failed");
            exit(EXIT_FAILURE);
        }
        else if (response == 0)
        {
            continue;
        }
        else
        {
            response = recvfrom(sockfd, (char *)buffer, BUFFSIZE, 0, (struct sockaddr *)&servaddr, &len);
            printf("Server time: %s", buffer);
            break;
        }
    }

    // If theres no timeout, the response would be > 0
    if(response == 0){
        printf("Timeout exceeded\n");
    }

    close(sockfd);

    return 0;
}