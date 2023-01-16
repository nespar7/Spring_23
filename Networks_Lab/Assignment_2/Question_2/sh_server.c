#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <dirent.h>

#define PORT 20000
#define BUFFSIZE 50

char *users_file = "users.txt";

int send_data(int sockfd, const char *buff, size_t buffer_size){

    printf("Send data function started\n");

    int sent = 0;
    int response;

    while(1){
        response = send(sockfd, buff+sent, 100, 0);
        if(response < -1) {
            break;
        }
        sent += response;

        if(sent >= buffer_size) break;
    }

    return (response < 0) ? -1 : sent;
}

char *receive_string(int sockfd)
{
    printf("Receive string function started\n");

    char buff[BUFFSIZE];
    char *received_string;
    int received_size = 50;
    int len = 0;
    int response;

    received_string = (char *)malloc(sizeof(char) * received_size);
    memset(received_string, 0, strlen(received_string));

    while (1)
    {
        response = recv(sockfd, buff, 50, 0);
        printf("Printing buff: %s\n", buff);
        printf("Received_string: %s\n", received_string);
        buff[response] = '\0';
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

    printf("%s\n", received_string);
    return received_string;
}

int find_word(char *word)
{
    printf("Find word function started\n");

    char line[1024];

    FILE *fp = fopen(users_file, "r");

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        if (!strcmp(line, word))
        {
            return 1;
        }
    }

    return 0;
}

void dir(int sockfd)
{
    printf("Dir function started\n");

    struct dirent *de;

    DIR *dr = opendir(".");

    char *result;
    int result_size = 50;
    int len = 0;

    result = (char *)malloc(sizeof(char) * result_size);

    if (dr == NULL) return;

    while ((de = readdir(dr)) != NULL)
    {
        while (result_size < strlen(de->d_name) + 1 + len)
        {
            result_size += 50;
        }

        printf("%d\n", result_size);
        result = realloc(result, result_size);

        strcat(result, de->d_name);
        strcat(result, " ");
    }

    printf("%s\n", result);
    send(sockfd, "HELLO", 6, 0);

    closedir(dr);
    free(result);
}

int main()
{
    int sockfd, newsockfd;
    int clilen;
    struct sockaddr_in servaddr, cliaddr;
    int response;
    char buff[BUFFSIZE];

    // Creating the socket sockfd
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    // bind
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (response < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(sockfd, 5);

    // Concurrent server
    while (1)
    {
        clilen = sizeof(cliaddr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);

        if (newsockfd < 0)
        {
            perror("Accept failed");
            close(newsockfd);
            exit(EXIT_FAILURE);
        }

        if (fork() == 0)
        {
            close(sockfd);
            strcpy(buff, "LOGIN:");

            response = send(newsockfd, buff, strlen(buff) + 1, 0);
            if (response < 0)
            {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }

            char *username = receive_string(newsockfd);
            printf("searching for %s...\n", username);

            if (find_word(username))
            {
                strcpy(buff, "FOUND");
            }
            else
            {
                strcpy(buff, "NOT-FOUND");
            }
            printf("%s %s\n\n", username, buff);

            response = send(newsockfd, buff, strlen(buff) + 1, 0);
            if (response < 0)
            {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }

            if (!strcmp(buff, "NOT-FOUND"))
            {
                exit(0);
            }

            free(username);

            while (1)
            {
                char *cmd = receive_string(newsockfd);
                printf("Received command: %s\n", cmd);

                if (!strcmp(cmd, "pwd"))
                {
                    char result[PATH_MAX];

                    if (getcwd(result, sizeof(result)) != NULL)
                    {
                        printf("%s\n", result);
                        response = send(newsockfd, result, strlen(result) + 1, 0);
                        if (response < 0)
                        {
                            perror("could not send pwd");
                            exit(0);
                        }
                    }
                    else
                    {
                        strcpy(result, "####");
                        response = send(newsockfd, result, strlen(result) + 1, 0);
                        if (response < 0)
                        {
                            perror("could not send pwd");
                            exit(0);
                        }
                    }
                }
                else if (!strcmp(cmd, "dir"))
                {
                    dir(newsockfd);
                }
                else if (cmd[0] == 'c' && cmd[1] == 'd')
                {
                    cmd += 3;
                    if (chdir((const char *)cmd) != 0)
                    {
                        char result[50];
                        strcpy(result, "####");
                        response = send(newsockfd, result, strlen(result) + 1, 0);
                        if (response < 0)
                        {
                            perror("could not send pwd");
                            exit(0);
                        }
                    }
                    else
                    {
                        char result[PATH_MAX];
                        if (getcwd(result, sizeof(result)) != NULL)
                        {
                            printf("%s\n", result);
                            response = send(newsockfd, result, strlen(result) + 1, 0);
                            if (response < 0)
                            {
                                perror("could not send pwd");
                                exit(0);
                            }
                        }
                        else
                        {
                            strcpy(result, "####");
                            response = send(newsockfd, result, strlen(result) + 1, 0);
                            if (response < 0)
                            {
                                perror("could not send error message");
                                exit(0);
                            }
                        }
                    }
                }
                else if (!strcmp(cmd, "exit"))
                {
                    free(cmd);
                    break;
                }
                else
                {
                    char result[5];
                    strcpy(result, "$$$$");
                    response = send(newsockfd, result, strlen(result) + 1, 0);
                    if (response < 0)
                    {
                        perror("could not send pwd");
                        exit(0);
                    }
                }
            }

            close(newsockfd);
            exit(0);
        }

        close(newsockfd);
    }
}