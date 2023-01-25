#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFMAX 50

char *receive_string(int sockfd){
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

int main(int argc, char *argv[]){
    int sockfd;
    int response;
    struct sockaddr_in lbaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }

    lbaddr.sin_family = AF_INET;
    lbaddr.sin_port = htons(atoi(argv[1]));
    lbaddr.sin_addr.s_addr = INADDR_ANY;

    response = connect(sockfd, (struct sockaddr*)&lbaddr, sizeof(lbaddr));
    if(response < 0){
        perror("Cannot connect to load balancer");
        exit(0);
    }

    char *time_data = receive_string(sockfd);
    printf("Server time: %s\n", time_data);

    close(sockfd);
    free(time_data);

    return 0;
}