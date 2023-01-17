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

int send_data(int sockfd, const char *buff, size_t buffer_size)
{

    printf("Send data function started\n");

    int sent = 0;
    int response;

    while (1)
    {
        response = send(sockfd, buff + sent, 100, 0);
        if (response < -1)
        {
            break;
        }
        sent += response;

        if (sent >= buffer_size)
            break;
    }

    return (response < 0) ? -1 : sent;
}

void change_directory(int sockfd, char buff[BUFFSIZE], int is_end)
{
    printf("Entered chdir\n");

    char path[PATH_MAX];

    printf("%ld\n", strlen(buff));

    int j = 0;
    for (int i = 3; i < strlen(buff); i++)
    {
        printf("Entered for loop");
        path[j++] = buff[i];
        printf("%s\n", path);
        if (buff[i] == '/')
        {
            chdir(path);
            j = 0;
            memset(path, 0, strlen(path));
        }
    }

    if (!is_end)
    {
        printf("Hello\n");
        while (1)
        {
            int response = recv(sockfd, buff, BUFFSIZE, 0);
            if (response < 0)
            {
                perror("Receive error");
                exit(EXIT_FAILURE);
            }

            for (int i = 4; i < strlen(buff); i++)
            {
                path[j++] = buff[i];
                if (buff[i] == '/')
                {
                    chdir(path);
                    j = 0;
                    memset(path, 0, strlen(path));
                }
            }

            if (buff[response - 1] == '\0')
                break;
        }
    }

    printf("%s\n", path);
    chdir(path);

    return;
}

char *receive_username(int sockfd)
{
    int len = 0;
    int received_size = 100;
    char *received_string;

    char buff[BUFFSIZE];
    received_string = (char *)malloc(sizeof(char) * received_size);

    while (1)
    {
        int response = recv(sockfd, buff, BUFFSIZE, 0);
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

    return received_string;
}

char *receive_command(int sockfd)
{
    char buff[BUFFSIZE];
    char *received_string;
    int received_size = 3;
    int len = 0;
    int response;

    int cmd_received = 0;
    int cmd_state = 0;

    received_string = (char *)malloc(sizeof(char) * received_size);
    memset(received_string, 0, 3);

    response = recv(sockfd, buff, 50, 0);

    if (response < 0)
    {
        perror("Could not receive");
        exit(0);
    }

    char ch = buff[0];

    if (ch == 'e')
    {
        return "1";
    }
    else if (ch == 'p')
    {
        // Deal with pwd;

        char result[PATH_MAX];

        if (getcwd(result, sizeof(result)) == NULL)
        {
            send_data(sockfd, "####", 5);
        }
        else
        {
            send_data(sockfd, result, strlen(result) + 1);
        }

        // Also deal with if received length is less than 3
    }
    // else if (buff[0] == 'd')
    // {

    // }
    else if (buff[0] == 'c')
    {
        printf("%s\n", buff);
        change_directory(sockfd, buff, buff[response-1] == '\0');
        printf("Got out\n");

        char result[PATH_MAX];

        if (getcwd(result, sizeof(result)) == NULL)
        {
            send_data(sockfd, "####", 5);
        }
        else
        {
            send_data(sockfd, result, strlen(result) + 1);
        }
    }

    return NULL;
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
    printf("Socket created\n\n");

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

            char *username = receive_username(newsockfd);
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

            while (1)
            {
                char *cmd = receive_command(newsockfd);

                if (cmd != NULL)
                {
                    printf("%s logging out\n", username);
                    break;
                }
            }

            close(newsockfd);
            exit(0);
        }

        close(newsockfd);
    }
}