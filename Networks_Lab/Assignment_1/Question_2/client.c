#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 20000

// Function to take input of unknown size
char *takeInput(FILE *fp, size_t size)
{
    char *str;
    int ch;
    size_t len = 0;

    str = realloc(NULL, sizeof(*str) * size);
    if (!str)
        return str;
    ch = fgetc(fp);
    while (EOF != ch && ch != '\n')
    {
        str[len++] = ch;
        if (len == size)
        {
            size += 16;
            str = realloc(str, sizeof(*str) * (size));
            if (!str)
                return str;
        }

        ch = fgetc(fp);
    }
    str[len++] = '\0';

    return realloc(str, sizeof(*str) * len);
}

// Function to send large data
int send_data(int sockfd, const char *buff, size_t buffer_size){
    int sent = 0;
    int response;

    while(1){
        response = send(sockfd, buff+sent, 100, 0);
        if(response < -1) {
            break;
        }
        sent += response;

        if(sent >= buffer_size) break;
    }

    return (response < 0) ? -1 : sent;
}

int main()
{
    int sockfd;
    int len;
    int response;
    char *buff;

    struct sockaddr_in addr;

    while (1)
    {
        // Create socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            perror("Cannot create socket");
            exit(0);
        }
        printf("TCP client socket created :)\n");

        addr.sin_family = AF_INET;
        addr.sin_port = htons(SERVER_PORT);
        addr.sin_addr.s_addr = INADDR_ANY;

        // Connect to the server
        response = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        if (response < 0)
        {
            perror("Connection to server failed");
            close(sockfd);
            exit(0);
        }
        printf("Connected to server!\n");

        // Taking the expression as input
        printf("\nEnter the expression(Enter -1 if you wish to terminate): ");
        buff = takeInput(stdin, 10);

        // if input is -1, close the socket and exit
        if (!strcmp(buff, "-1"))
        {
            printf("Bye have a nice day!\n");
            close(sockfd);
            exit(0);
        }

        // Send the expression to the server
        response = send_data(sockfd, buff, strlen(buff) + 1);
        if (response < 0)
        {
            perror("Cannot send the expression to server");
            close(sockfd);
            exit(0);
        }
        printf("\nExpression sent!\n");

        // Receive the evaluated value from the server
        response = recv(sockfd, buff, strlen(buff) + 1, 0);
        if (response < 0)
        {
            perror("Cannot receive from server");
            close(sockfd);
            exit(0);
        }
        printf("Result: %s\n\n", buff);

        // Free the space allocated for buff and close the socket
        free(buff);
        close(sockfd);
    }

    close(sockfd);
}