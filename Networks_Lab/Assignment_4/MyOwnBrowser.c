#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

// For now I assume that the ip address and host are given in the arguments
int main(int argc, char *argv[]){
    int browser_sockfd;
    int response;
    struct sockaddr_in serv_addr;

    browser_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(browser_sockfd < 0){
        perror("Cannot create client socket");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("MyOwnBrowser> ");

        char *prompt = takeInput(stdin, 50);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(atoi(argv[2]));
        inet_aton(argv[1], &serv_addr.sin_addr.s_addr);

        response = connect(browser_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if(response < 0){
            perror("Cannot connect to server");
            close(browser_sockfd);
            exit(EXIT_FAILURE);
        }

    }
}