#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 20000

char *takeInput(FILE *fp, size_t size){
    char *str;
    int ch;
    size_t len = 0;

    str = realloc(NULL, sizeof(*str)*size);
    if(!str)return str;
    ch=fgetc(fp);
    while(EOF!=ch && ch != '\n'){
        str[len++] = ch;
        if(len == size){
            size+=16;
            str = realloc(str, sizeof(*str)*(size));
            if(!str)return str;
        }

        ch = fgetc(fp);
    }
    str[len++] = '\0';

    return realloc(str, sizeof(*str)*len);
}

int main(){
    int sockfd;
    int len;
    int response;
    char *buff;

    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("TCP client socket created :)\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    response = connect(sockfd, (struct sockaddr*) &addr, sizeof(addr));
    if(response < 0){
        perror("Connection to server failed");
        close(sockfd);
        exit(0);
    }
    printf("Connected to server!\n");

    printf("Enter the expression: ");
    buff = takeInput(stdin, 10);
    printf("%s\n", buff);

    response = send(sockfd, buff, strlen(buff)+1, 0);
    if(response < 0){
        perror("Cannot send the expression to server");
        close(sockfd);
        exit(0);
    }
    printf("Expression sent!\n");

    response = recv(sockfd, buff, strlen(buff)+1, 0);
    if(response < 0){
        perror("Cannot receive from server");
        close(sockfd);
        exit(0);
    }
    printf("Result: %s\n", buff);

    free(buff);

    close(sockfd);
}