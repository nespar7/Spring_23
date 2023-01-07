/**********************************************************************************************/ 
/* References:                                                                                */
/*  https://www.ibm.com/docs/en/i/7.1?topic=designs-example-writing-iterative-server-program  */
/*                                                                                            */
/*  https://forgetcode.com/c/1476-tcp-socket-date-and-time                                    */
/**********************************************************************************************/

// include statements
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 20000

int main(){
    int sockfd, newsockfd; 
    int clilen;
    int response;
    struct sockaddr_in serv_addr, cli_addr;

    int i;
    char buff[100];

    // opening a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket!\n");
        exit(0);
    }
    printf("TCP server socket created :)\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    // inet_aton(INADDR_ANY, &serv_addr.sin_addr);?
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding the server address to the socket
    response = bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if(response < 0){
        perror("Unable to bind localhost!\n");
        close(sockfd);
        exit(0);
    }
    printf("Bound to port number: 20000\n");

    // We listen to upto 5 servers at a time in the queue
    response = listen(sockfd, 5);
    if(response < 0){
        perror("Failed to listen!\n");
        close(sockfd);
        exit(0);
    }
    printf("Listening...\n");

    // iterative server
    while(1){
        clilen = sizeof(cli_addr);

        // Accepting a client connection
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);

        if(newsockfd < 0){
            perror("Accept failed!\n");
            close(sockfd);
            exit(0);
        }

        // Storing server time in buff
        time_t t;
        t = time(NULL);
        snprintf(buff, sizeof(buff), "%s", ctime(&t));

        // Sending the server time message
        response = send(newsockfd, buff, strlen(buff)+1, 0);

        if(response < 0){
            perror("Failed to send the message!\n");
            close(sockfd);
            close(newsockfd);
            exit(0);
        }

        close(newsockfd);
        // No receive call because the client does not send any data
    }

    close(sockfd);
}