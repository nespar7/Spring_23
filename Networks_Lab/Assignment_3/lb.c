#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/time.h>

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

// argv[1] - load balancer port, argv[2] - server 1 port, argv[3] - server 2 port
int main(int argc, char *argv[]){
    int lbsockfd, newsockfd;
    int response;
    struct sockaddr_in lbaddr, server_1, server_2, client;
    char *time_request = "Send Time";
    char *load_request = "Send Load";
    double timeout = 5000;
    struct timeval poll_start, poll_end;

    // Create a socket
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

    // Bind the load balancer socket since it acts like a server to the client
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
    printf("Listening at port: %s...\n\n", argv[1]);

    int server_load_1 = 0, server_load_2 = 0;

    // Concurrent server
    while(1){
        struct pollfd setfd;

        setfd.fd = lbsockfd;
        setfd.events = POLLIN;

        // poll_start keeps the time before starting the poll
        gettimeofday(&poll_start, NULL);
        response = poll(&setfd, 1, timeout);

        // If poll returns -1, error
        if(response < 0){
            perror("Poll error");
            exit(0);
        }
        // If poll returns zero, timeout
        else if(response == 0){
            // Set the timeout to 5 seconds again
            timeout = 5000;

            // creating sockets for server 1 and 2
            int loadsockfd_1, loadsockfd_2;
            loadsockfd_1 = socket(AF_INET, SOCK_STREAM, 0);
            loadsockfd_2 = socket(AF_INET, SOCK_STREAM, 0);

            // Connect to server 1, send the "Send Load" message and store the load in server_load_1
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
            printf("Server load at port %s: %d\n", argv[2], server_load_1);

            // Connect to server 2, send the "Send Load" message and store the load in server_load_2
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
            printf("Server load at port %s: %d\n", argv[3], server_load_2);

            // Closing the sockets
            close(loadsockfd_1);
            close(loadsockfd_2);
        }
        else{
            // Accept the client connection
            int clilen = sizeof(client);
            newsockfd = accept(lbsockfd, (struct sockaddr*)&client, &clilen);
            if(newsockfd < 0){
                perror("Accept error");
                exit(0);
            }
            printf("Connected to client\n");

            // The child process gets the time and sends it to the server
            if(fork() == 0){
                close(lbsockfd);
                int servsockfd = socket(AF_INET, SOCK_STREAM, 0);

                // If server 1 has less load connect to it, else connect to server 2
                int server;
                if(server_load_1 <= server_load_2){
                    response = connect(servsockfd, (struct sockaddr*)&server_1, sizeof(server_1));
                    if(response < 0){
                        perror("Cannot connect to server 1");
                        exit(0);
                    }
                    server = atoi(argv[2]);
                }
                else{
                    response = connect(servsockfd, (struct sockaddr*)&server_2, sizeof(server_2));
                    if(response < 0){
                        perror("Cannot connect to server 2");
                        exit(0);
                    }
                    server = atoi(argv[3]);
                }

                // Send the "Send Time" request
                response = send_data(servsockfd, time_request, strlen(time_request)+1);
                if(response < 0){
                    perror("Cannot send time request to server");
                    exit(0);
                }

                printf("Sending time request to server at port %d\n\n", server);
                // Receiving the time as a string
                char *time_data = receive_string(servsockfd);
                
                // Close the socket
                close(servsockfd);

                // Sending the time to client
                response = send_data(newsockfd, time_data, strlen(time_data)+1);
                if(response < 0){
                    perror("Cannot send time data to client\n");
                }
                
                exit(0);
            }

            // poll_end is the time that the poll() call waits (approximate since the subsequent calls take some time)
            gettimeofday(&poll_end, NULL);
            // The timeout decreases by the time that poll waited
            timeout -= ((poll_end.tv_sec - poll_start.tv_sec) + (poll_end.tv_usec - poll_start.tv_usec)/1000000.0)*1000;
            // If for any reason the timeout is negative(might be due to 
            // the subsequent calls taking a bit more time than timeout), set timeout to 0 
            if(timeout < 0){
                timeout = 0;
            }

            close(newsockfd);
        }
        printf("Timeout: %f\n\n", timeout);
    }

}