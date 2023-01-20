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

int minimum(int x)
{
    return x < BUFFSIZE ? x : BUFFSIZE;
}

int send_data(int sockfd, char *buffer, int bufsize)
{
    const char *pbuffer = (const char *)buffer;
    while (bufsize > 0)
    {
        int response = send(sockfd, pbuffer, minimum(bufsize), 0);
        if (response < 0)
            return -1;
        pbuffer += response;
        bufsize -= response;
    }
    return 0;
}

char *receive_string(int sockfd)
{
    char buff[BUFFSIZE];
    char *received_string;
    int response;

    received_string = (char *)malloc(sizeof(char) * BUFFSIZE);
    int stored = 0;
    int allocated = BUFFSIZE;

    while (1)
    {
        response = recv(sockfd, buff, BUFFSIZE, 0);

        if (response < 0)
        {
            perror("Cannot receive data");
            close(sockfd);
            exit(0);
        }

        int changed = 0;
        while(stored + response > allocated){
            allocated += BUFFSIZE;
            changed = 1;
        }

        if(changed){
            received_string = realloc(received_string, allocated);
        }
    
        strcat(received_string, buff);
        stored += response;

        if (buff[response - 1] == '\0')
        {
            break;
        }
    }

    return received_string;
}

char *removeSpaces(char *str)
{
    int len = 0;
    int space_encountered = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if(!space_encountered){
            str[len++] = str[i];
            if(str[i] == ' ' || str[i] == '\t'){
                while(str[i] == ' ' || str[i] == '\t'){
                    i++;
                }
                space_encountered = 1; 
                i--;
            }
        }
        else{
            str[len++] = str[i];
        }

    }

    if(str[len-1] == ' ' || str[len-1] == '\t'){
        str[len-1] = '\0';
    }
    str[len] = '\0';
    return str;
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

void list_dir(int sockfd, char *arg){
    DIR *dir;
    struct dirent *entry;

    dir = opendir(arg);

    if(dir == NULL){
        send_data(sockfd, "####", 5);
    }
    else{
        char *result;
        result = (char*)malloc(sizeof(char)*BUFFSIZE);
        int stored = 0;
        int allocated = BUFFSIZE;

        entry = readdir(dir);
        while(entry != NULL){
            char *name = entry->d_name;
            if(!strcmp(name, ".") || !strcmp(name, "..")) {
                entry = readdir(dir);
                continue;
            }
            int entry_length = strlen(name);

            int changed = 0;
            while(stored + entry_length + 2 > allocated){
                allocated += BUFFSIZE;
                changed = 1;
            }

            if(changed){
                result = realloc(result, allocated);
            }

            strcat(result, name);
            stored += entry_length;

            result[stored++] = ' ';
            result[stored++] = ' ';

            entry = readdir(dir);
        }

        int response = send_data(sockfd, result, strlen(result)+1);
    }
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
    printf("Created socket\n");

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

            response = send_data(newsockfd, buff, strlen(buff) + 1);
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

            response = send_data(newsockfd, buff, strlen(buff) + 1);
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
                cmd = removeSpaces(cmd);

                if(strlen(cmd) < 2){
                    response = send_data(newsockfd, "####", 5);
                }
                else if (!strcmp(cmd, "pwd"))
                {
                    char result[PATH_MAX];

                    if (getcwd(result, sizeof(result)) != NULL)
                    {
                        response = send_data(newsockfd, result, strlen(result) + 1);
                    }
                    else
                    {
                        response = send_data(newsockfd, "####", 5);
                    }
                }
                else if (strlen(cmd) >= 3 && cmd[0] == 'd' && cmd[1] == 'i' && cmd[2] == 'r')
                {
                    if(strlen(cmd) == 3){
                        list_dir(newsockfd, ".");
                    }
                    else{
                        list_dir(newsockfd, cmd+4);
                    }
                }
                else if (strlen(cmd) >= 2 && cmd[0] == 'c' && cmd[1] == 'd')
                {

                    if (strlen(cmd) == 2)
                    {
                        if (chdir("/home") != 0)
                        {
                            response = send_data(newsockfd, "####", 5);
                        }
                        else
                        {
                            response = send_data(newsockfd, "1", 2);
                        }
                    }
                    else if (cmd[2] == ' ')
                    {
                        // Deal with chdir
                        if (change_dir(cmd + 3) != 0)
                        {
                            response = send_data(newsockfd, "####", 5);
                        }
                        else
                        {
                            response = send_data(newsockfd, "1", 2);
                        }
                    }
                    else
                    {
                        response = send_data(newsockfd, "$$$$", 5);
                    }
                }
                else if (!strcmp(cmd, "exit"))
                {
                    printf("%s logging out\n", username);
                    break;
                }
                else{
                    response = send_data(newsockfd, "$$$$", 5);
                }
            }

            close(newsockfd);
            exit(0);
        }
    }
}