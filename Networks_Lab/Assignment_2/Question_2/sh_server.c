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
#define RECSIZE 200

char *users_file = "users.txt";

char *receive_string(int sockfd)
{
    char buff[BUFFSIZE];
    char *received_string;
    int response;

    received_string = (char *)malloc(sizeof(char) * RECSIZE);

    while (1)
    {
        response = recv(sockfd, buff, 50, 0);

        if (response < 0)
        {
            perror("Cannot receive data");
            close(sockfd);
            exit(0);
        }

        strcat(received_string, buff);

        if (buff[response - 1] == '\0')
        {
            break;
        }
    }

    return received_string;
}

int find_word(char *word)
{
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

int change_dir(char *s)
{
    if (chdir(s) != 0)
    {
        return -1;
    }

    return 0;
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
                    }
                    else
                    {
                        response = send(newsockfd, "####", 5, 0);
                    }
                }
                else if (!strcmp(cmd, "dir"))
                {
                    printf("HELLO");
                }
                else if (cmd[0] == 'c' && cmd[1] == 'd')
                {

                    if (strlen(cmd) == 2)
                    {
                        if (chdir("/home") != 0)
                        {
                            response = send(newsockfd, "####", 5, 0);
                        }
                        else
                        {
                            response = send(newsockfd, "cd", 3, 0);
                        }
                    }
                    else if (cmd[2] == ' ')
                    {
                        // Deal with chdir
                        if (change_dir(cmd + 3) != 0)
                        {
                            response = send(newsockfd, "####", 5, 0);
                        }
                        else
                        {
                            response = send(newsockfd, "cd", 3, 0);
                        }
                    }
                    else
                    {
                        response = send(newsockfd, "$$$$", 5, 0);
                    }
                }
                else if (!strcmp(cmd, "exit"))
                {
                    printf("%s logging out\n", username);
                    free(cmd);
                    break;
                }
            }

            close(newsockfd);
            exit(0);
        }
    }
}