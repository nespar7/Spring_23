#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 20000
#define BUFFSIZE 50

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

int main(){
    int sockfd;
    int len;
    int response;
    char buff[BUFFSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    response = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(response < 0){
        perror("Could not connect to server");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server!\n");

    response = recv(sockfd, buff, BUFFSIZE, 0);
    if(response < 0){
        perror("Receive error");
        exit(EXIT_FAILURE);
    }   

    printf("%s", buff);

    response = send(sockfd, buff, BUFFSIZE, 0);

    close(sockfd);
}