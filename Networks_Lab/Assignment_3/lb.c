#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
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
    printf("Entered receive string fn\n");

    char buff[BUFFMAX];
    char *received_string;
    int response;

    // Initial allocation
    received_string = (char *)malloc(sizeof(char) * BUFFMAX);
    int stored = 0;
    int allocated = BUFFMAX;

    // while '\0' is not received iteratively receive and concatenate the buffer to received_string
    while (1)
    {
        response = recv(sockfd, buff, BUFFMAX, 0);
        printf("%s\n", buff);

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
    int lbsockfd, newsockfd;
    int response;
    struct sockaddr_in lbaddr, server_1, server_2, client;
    char *time_request = "Send Time";
    char *load_request = "Send Load";
    int timeout = 5000;
    time_t poll_start, poll_end;

    lbsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lbsockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("Socket created\n");

    lbaddr.sin_family = AF_INET;
    lbaddr.sin_port = htons(atoi(argv[1]));
    lbaddr.sin_addr.s_addr = INADDR_ANY;

    server_1.sin_family = AF_INET;
    server_1.sin_port = htons(atoi(argv[2]));
    server_1.sin_addr.s_addr = INADDR_ANY;

    server_2.sin_family = AF_INET;
    server_2.sin_port = htons(atoi(argv[3]));
    server_2.sin_addr.s_addr = INADDR_ANY;

    response = bind(lbsockfd, (struct sockaddr*)&lbaddr, sizeof(lbaddr));
    if(response < 0){
        perror("Bind error");
        exit(0);
    }

    response = listen(lbsockfd, 5);
    if(response < 0){
        perror("Listen error");
        exit(0);
    }
    printf("Listening at port: %s...\n", argv[1]);

    int server_load_1 = 0, server_load_2 = 0;

    // Concurrent server
    while(1){
        struct pollfd setfd;

        setfd.fd = lbsockfd;
        setfd.events = POLLIN;

        poll_start = time(NULL);
        response = poll(&setfd, 1, timeout);

        if(response < 0){
            perror("Poll error");
            exit(0);
        }
        else if(response == 0){
            timeout = 5000;

            int loadsockfd_1, loadsockfd_2;
            loadsockfd_1 = socket(AF_INET, SOCK_STREAM, 0);
            loadsockfd_2 = socket(AF_INET, SOCK_STREAM, 0);

            response = connect(loadsockfd_1, (struct sockaddr*)&server_1, sizeof(server_1));
            if(response < 0){
                perror("Cannot connect to server 1");
                exit(0);
            }

            response = send_data(loadsockfd_1, load_request, strlen(load_request)+1);
            if(response < 0){
                perror("Cannot send data to server 1");
                exit(0);
            }
            server_load_1 = atoi(receive_string(loadsockfd_1));
            printf("Server 1 load: %d\n", server_load_1);

            response = connect(loadsockfd_2, (struct sockaddr*)&server_2, sizeof(server_2));
            if(response < 0){
                perror("Cannot connect to server 2");
                exit(0);
            }
            response = send_data(loadsockfd_2, load_request, strlen(load_request)+1);
            if(response < 0){
                perror("Cannot send data to server 2");
                exit(0);
            }
            server_load_2 = atoi(receive_string(loadsockfd_2));
            printf("Server 2 load: %d\n", server_load_2);

            close(loadsockfd_1);
            close(loadsockfd_2);
        }
        else{
            int clilen = sizeof(client);
            newsockfd = accept(lbsockfd, (struct sockaddr*)&client, &clilen);
            if(newsockfd < 0){
                perror("Accept error");
                exit(0);
            }
            printf("Connected to client\n");

            if(fork() == 0){

                int servsockfd = socket(AF_INET, SOCK_STREAM, 0);

                if(server_load_1 <= server_load_2){
                    response = connect(servsockfd, (struct sockaddr*)&server_1, sizeof(server_1));
                    if(response < 0){
                        perror("Cannot connect to server 1");
                        exit(0);
                    }
                    printf("Sending time request to %s\n", inet_ntoa(server_1.sin_addr));
                }
                else{
                    response = connect(servsockfd, (struct sockaddr*)&server_2, sizeof(server_2));
                    if(response < 0){
                        perror("Cannot connect to server 2");
                        exit(0);
                    }
                    printf("Sending time request to %s\n", inet_ntoa(server_2.sin_addr));
                }

                response = send_data(servsockfd, time_request, strlen(time_request)+1);
                if(response < 0){
                    perror("Cannot send time request to server 1\n");
                    exit(0);
                }

                char *time_data = receive_string(servsockfd);

                response = send_data(newsockfd, time_data, strlen(time_data)+1);
                if(response < 0){
                    perror("Cannot send time data to client\n");
                }
                
                close(servsockfd);
                exit(0);
            }

            poll_end = time(NULL);
            timeout -= difftime(poll_end, poll_start)*1000;
            if(timeout < 0){
                timeout = 0;
            }
        }
    }

}