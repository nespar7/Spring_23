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
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server!\n");

    response = recv(sockfd, buff, BUFFSIZE, 0);
    if(response < 0){
        perror("Could not receive login message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }   

    printf("%s ", buff);

    char *username = takeInput(stdin, 50);

    response = send(sockfd, username, strlen(username)+1, 0);
    if(response < 0){
        perror("Could not send username");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    response = recv(sockfd, buff, BUFFSIZE, 0);
    if(response < 0){
        perror("Could not receive username status");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if(!strcmp(buff, "NOT-FOUND")){
        printf("Invalid username\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Hello %s\n\n", username);

    while(1){
        printf("Enter command: ");
        char *cmd = takeInput(stdin, 50);


        response = send(sockfd, cmd, strlen(cmd)+1, 0);
        if(response < 0){
            perror("Could not send command");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        if(!strcmp(cmd, "exit")){
            printf("Bye have a nice day :)\n");
            exit(0);
        }

        char *result = receive_string(sockfd);

        printf("%s\n", result);
    }

    close(sockfd);
}