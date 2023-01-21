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

int minimum(int x)
{
    return x < BUFFSIZE ? x : BUFFSIZE;
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
    char buff[BUFFSIZE];
    char *received_string;
    int response;

    // Initial allocation
    received_string = (char *)malloc(sizeof(char) * BUFFSIZE);
    int stored = 0;
    int allocated = BUFFSIZE;

    // while '\0' is not received iteratively receive and concatenate the buffer to received_string
    while (1)
    {
        response = recv(sockfd, buff, BUFFSIZE, 0);

        if (response < 0)
        {
            perror("Cannot receive data");
            close(sockfd);
            exit(0);
        }

        // If the allocated size is not enough to concatenate the buffer, increase it and reallocate
        int changed = 0;
        while(stored + response > allocated){
            allocated += BUFFSIZE;
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

// This function removes the first burst of spaces and replace it with a single space
// example: dir        Question_2 -> dir Question_2
char *removeSpaces(char *str)
{
    int len = 0;
    int space_encountered = 0;

    // If str[i] is the first space encountered, ignore the whole burst of whitespaces
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

    // Removing trailing whitespaces
    if(str[len-1] == ' ' || str[len-1] == '\t'){
        str[len-1] = '\0';
    }
    str[len] = '\0';
    return str;
}

// Function to find a word in a given file
int find_word(char *word, char *users_file)
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

// Function to change into the directory name given by s
int change_dir(char *s)
{
    // If the chdir function gives and error, send -1 to the caller
    if (chdir(s) != 0)
    {
        return -1;
    }

    // If no error, send 0
    return 0;
}

// Handling the dir command
void list_dir(int sockfd, char *arg){
    DIR *dir;
    struct dirent *entry;

    // Opening the directory specified by the arguments
    dir = opendir(arg);

    // If directory could not be opened, send running error to client
    if(dir == NULL){
        send_data(sockfd, "####", 5);
    }
    else{
        // Take a result pointer and allocate it an initial size of 50
        char *result;
        result = (char*)malloc(sizeof(char)*BUFFSIZE);
        int stored = 0;
        int allocated = BUFFSIZE;

        // Keep reading from the directory until there is none left
        entry = readdir(dir);
        while(entry != NULL){
            // Get the name of the entry
            char *name = entry->d_name;
            // Since the dir command omits .(current directory) and ..(previous directory), I have also omitted them
            if(!strcmp(name, ".") || !strcmp(name, "..")) {
                entry = readdir(dir);
                continue;
            }
            
            // Similar to the receive_string function, reallocate memory to the result string whenever necessary
            int entry_length = strlen(name);

            int changed = 0;
            while(stored + entry_length + 2 > allocated){
                allocated += BUFFSIZE;
                changed = 1;
            }

            if(changed){
                result = realloc(result, allocated);
            }

            // Concatenate the entry name to result and add two spaces(linux does this in case of any files starting with ')
            strcat(result, name);
            stored += entry_length;

            result[stored++] = ' ';
            result[stored++] = ' ';

            entry = readdir(dir);
        }

        // send result to the client
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

    // bind to local address
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

        // The child process takes care of running the commands, while the parent gets blocked at the accept call
        if (fork() == 0)
        {
            // close sockfd since the child process does not need it
            close(sockfd);

            // Send LOGIN to client
            strcpy(buff, "LOGIN:");

            response = send_data(newsockfd, buff, strlen(buff) + 1);
            if (response < 0)
            {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }

            // Receive the username and search for it in the file
            char *username = receive_string(newsockfd);
            printf("searching for %s...\n", username);

            char *users_file = "users.txt";

            if (find_word(username, users_file))
            {
                strcpy(buff, "FOUND");
            }
            else
            {
                strcpy(buff, "NOT-FOUND");
            }

            // send search result to client
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

            // Keep receiving commands from the client until an exit is received
            while (1)
            {
                // Receive the command
                char *cmd = receive_string(newsockfd);
                cmd = removeSpaces(cmd);

                int len = strlen(cmd);

                // None of the valid commands are 1 or 0 length, so send invalid function
                if(len < 2){
                    response = send_data(newsockfd, "$$$$", 5);
                }
                // For pwd handling
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
                // For dir handling
                else if ((len == 3 && cmd[0] == 'd' && cmd[1] == 'i' && cmd[2] == 'r') || (len >= 3 && cmd[0] == 'd' && cmd[1] == 'i' && cmd[2] == 'r' && cmd[3] == ' '))
                {
                    // If the command is simply dir, open the current directory
                    if(strlen(cmd) == 3){
                        list_dir(newsockfd, ".");
                    }
                    // Else send the argunent
                    else{
                        list_dir(newsockfd, cmd+4);
                    }
                }
                // For cd handling
                else if (len >= 2 && cmd[0] == 'c' && cmd[1] == 'd')
                {
                    // If the command is cd, change to home
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
                // Exit handling
                else if (!strcmp(cmd, "exit"))
                {
                    printf("%s logging out\n\n", username);
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