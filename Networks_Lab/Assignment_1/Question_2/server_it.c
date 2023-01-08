#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 20000
#define max 100;

char peek(char **s);
char get(char **s);
float expression(char **s);
float term(char **s);
float factor(char **s);
float number(char **s);
float dec_number(char **s);

int main(){
    int sockfd, newsockfd;
    int clilen;
    int response;
    struct sockaddr_in serv_addr, cli_addr;

    int i;
    char buff[100];
    
    // Opening a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("TCP server socket created :)\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(response < 0){
        perror("Unable to bind to localhost");
        exit(0);
    }
    printf("Bound to port number: %d\n", SERVER_PORT);

    response = listen(sockfd, 5);
    if(response < 0){
        perror("Listen failed");
        exit(0);
    }
    printf("Listening...\n");

    while(1){
        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if(newsockfd < 0){
            perror("Accept failed");
            exit(0);
        }

        int offset = 0;
        while(1){
            response = recv(newsockfd, buff+offset, 100, 0);

            if(response < 0){
                perror("Cannot receive data");
                close(newsockfd);
                exit(0);
            }

            if(buff[offset+response-1] == '\0'){
                break;
            }

            offset += response;
        }
        printf("Received expression: %s\n", buff);
        printf("Received bytes: %d\n", response);

        // DEAL WITH THE ARITHMETIC HERE
        // char *result;
        // strcpy(result, buff);
        // printf("HELLO\n");
        // float value = expression(&result);
        // printf("%f\n", value);
        // printf("HELLO\n");

        // sprintf(buff, "%f", value);

        response = send(newsockfd, buff, strlen(buff)+1, 0);
        if(response < 0){
            perror("Cannot send the result");
            close(newsockfd);
            exit(0);
        }

        close(newsockfd);
    }

    close(sockfd);
}

char peek(char **s){
    return **s;
}

char get(char **s){
    return *((*s)++);
}

float number(char **s){
    float result = (get(s) - '0');

    char ch = peek(s);
    while(ch >= '0' && ch <= '9'){
        result = 10*result + (get(s) - '0');

        ch = peek(s);
    }

    return result;
}

float dec_number(char **s){
    float result = (get(s)-'0')/10;
    float mul = 100;

    char ch = peek(s);    
    while(ch >= '0' && ch <= '9'){
        result += (get(s) - '0')/mul;
        mul *= 10;

        ch = peek(s);
    }

    return result;
}

float factor(char **s){
    char ch = peek(s);
    
    if(ch == '('){
        get(s);
        float result = expression(s);
        get(s);
        return result;
    }
    else if(ch == '-'){
        get(s);
        return -factor(s); 
    }
    else if(ch >= '0' && ch <= '9'){
        float result = number(s);
        ch = peek(s);
        if(ch == '.'){
            get(s);
            result += dec_number(s);
        }
        return result;
    }

    return 0;
}

float term(char **s){
    float result = factor(s);
    char ch = peek(s);
    
    while(ch == '*' || ch == '/'){
        if(get(s) == '*'){
            result *= factor(s);
        }
        else{
            result /= factor(s);
        }

        ch = peek(s);
    }

    return result;
}

float expression(char **s){
    float result = term(s);

    char ch = peek(s);
    while(ch == '+' || ch == '-'){
        if(get(s) == '+'){
            result += term(s);
        }
        else{
            result -= term(s);
        }

        ch = peek(s);
    }

    return result;
}