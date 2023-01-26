#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define BUFFMAX 50

int minimum(int x)
{
    return x < BUFFMAX ? x : BUFFMAX;
}

// function to send data in chunks of 50 or less than 50
int send_data(int sockfd, char *buffer, int buffsize)
{
    const char *temp = (const char *)buffer;

    // while buffer still has data, send 50 or buffsize(minimum of) number of bytes
    while (buffsize > 0)
    {
        int response = send(sockfd, temp, minimum(buffsize), 0);
        if (response < 0) return -1;

        // increment the temp string to the position of the next character to be sent
        temp += response;
        buffsize -= response;
    }
    return 0;
}

// Function to receive data in chunks of less than 50 size
char *receive_string(int sockfd)
{
    char buff[BUFFMAX];
    char *received_string;
    int response;

    // Initial allocation
    received_string = (char *)malloc(sizeof(char) * BUFFMAX);
    memset(received_string, 0, BUFFMAX);
    int stored = 0;
    int allocated = BUFFMAX;

    // while '\0' is not received iteratively receive and concatenate the buffer to received_string
    while (1)
    {
        response = recv(sockfd, buff, BUFFMAX, 0);

        if (response < 0)
        {
            perror("Cannot receive data");
            close(sockfd);
            exit(0);
        }

        // If the allocated size is not enough to concatenate the buffer, increase it and reallocate
        int changed = 0;
        while(stored + response > allocated){
            allocated += BUFFMAX;
            changed = 1;
        }

        if(changed){
            received_string = realloc(received_string, allocated);
        }
    
        // concatenate the buffer to received_string
        strcat(received_string, buff);
        stored += response;

        if (buff[response - 1] == '\0')
        {
            break;
        }
    }

    return received_string;
}

int main(int argc, char *argv[]){
    int sockfd, newsockfd;
    int clilen;
    int response;
    struct sockaddr_in servaddr, cliaddr;
    int port_no = atoi(argv[1]);
    srand((unsigned) port_no);

    char buff[BUFFMAX];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("Created socket\n");

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port_no);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(response < 0){
        perror("Could not bind");
        exit(0);
    }

    response = listen(sockfd, 5);
    if(response < 0){
        perror("Listen error");
        exit(0);
    }
    printf("Listening at port %s...\n\n", argv[1]);


    // Iterating
    while(1){
        int newsockfd;
        
        clilen = sizeof(cliaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
        if(newsockfd < 0){
            perror("Accept error");
            exit(0);
        }

        char *received = receive_string(newsockfd);

        if(!strcmp(received, "Send Load")){
            int num = rand() % 100 + 1;
            sprintf(received, "%d", num);
            response = send_data(newsockfd, received, strlen(received)+1);
            if(response < 0){
                perror("Cannot send load");
                exit(0);
            }
            printf("Load sent: %d\n\n", num);
        }
        else if(!strcmp(received, "Send Time")){
            time_t t;
            t = time(NULL);
            char *message = ctime(&t);
    
            response = send_data(newsockfd, message, strlen(message)+1);
            if(response < 0){
                perror("Cannot send time");
                exit(0);
            }
            printf("Time sent: %s\n\n", message);
        }   
        else{
            response = send_data(newsockfd, "Invalid request", 16);
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}