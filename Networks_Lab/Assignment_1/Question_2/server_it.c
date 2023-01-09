#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 20000

// Forward declarations
char peek(char **s);
char get(char **s);
float expression(char **s);
float term(char **s);
float number(char **s);
float dec_number(char **s);
char *removeSpaces(char *str);

int main(){
    int sockfd, newsockfd;
    int clilen;
    int response;
    struct sockaddr_in serv_addr, cli_addr;

    int i;
    char buff[100]; 
    
    // Opening a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("Cannot create socket");
        exit(0);
    }
    printf("TCP server socket created :)\n");

    // Binding the socket to localhost port 20000
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    response = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(response < 0){
        perror("Unable to bind to localhost");
        exit(0);
    }
    printf("Bound to port number: %d\n", SERVER_PORT);

    // Listen
    response = listen(sockfd, 5);
    if(response < 0){
        perror("Listen failed");
        exit(0);
    }
    printf("Listening...\n");

    // Iterative server
    while(1){
        clilen = sizeof(cli_addr);

        // Connecting to client
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if(newsockfd < 0){
            perror("Accept failed");
            exit(0);
        }

        /********************************************************************************/    
        /* The below code is to receive the unknown length string from the client       */
        /* Initialise the received_string string to a length of 100                     */
        /* Receive data(100 bytes max) and store in the buff                            */
        /* If the length of buff is lesser than the memory allocated to received_string */
        /* reallocate memory for the received_string until buff can be accepted         */
        /* when the last character of buff is '\0' break the loop                       */
        /********************************************************************************/
        int len = 0;
        int received_size = 100;
        char *received_string;

        received_string = (char *)malloc(sizeof(char) * received_size);

        while(1){
            response = recv(newsockfd, buff, 100, 0);

            if(response < 0){
                perror("Cannot receive data");
                close(newsockfd);
                exit(0);
            }

            while(len + response >= received_size){
                received_size += 100;
            }

            received_string = realloc(received_string, received_size);

            strcat(received_string, buff);

            if(buff[response-1] == '\0'){
                break;
            }
        }
        printf("Received expression: %s\n", received_string);

        // process the expression to remove whitespaces
        received_string = removeSpaces(received_string);
        printf("despaced string: %s\n", received_string);

        // Evaluate the expression and store the evaluated value into received_string
        float value = expression(&received_string);
        sprintf(received_string, "%f", value);

        // send the evaluated value to the client
        response = send(newsockfd, received_string, strlen(received_string)+1, 0);
        if(response < 0){
            perror("Cannot send the result");
            close(newsockfd);
            exit(0);
        }

        // close newsockfd
        close(newsockfd);
    }

    close(sockfd);
}

// Function to remove white spaces from the string
char *removeSpaces(char *str){
    int len = 0;

    for(int i = 0; str[i] != '\0';i++){
        if(str[i] != ' ' && str[i] != '\t'){
            str[len] = str[i];
            len++;
        }
    }

    str[len] = '\0';
    return str;
}

// Returns the character at the start of the string
char peek(char **s){
    return **s;
}

// Returns the character at the start of the string and increments the pointer to the string by 1
char get(char **s){
    return *((*s)++);
}

/********************************************************************************************/
/* The grammar used is:                                                                    	*/
/* E -> T | T+T | T-T | T*T | T/T 															*/
/* T -> (E) | -T | number | number.dec_num													*/
/********************************************************************************************/

float number(char **s){
    float result = (get(s) - '0');

	// While the next character is a digit, number = number * 10 + digit
    char ch = peek(s);
    while(ch >= '0' && ch <= '9'){
        result = 10*result + (get(s) - '0');

        ch = peek(s);
    }

    return result;
}

float dec_number(char **s){
    float result = ((float)(get(s)-'0'))/10;
    float mul = 100;

    char ch = peek(s);    
    while(ch >= '0' && ch <= '9'){
        result += ((float)(get(s) - '0'))/mul;
        mul *= 10;

        ch = peek(s);
    }

    return result;
}

float term(char **s){
    char ch = peek(s);

	// T -> (E)
    if(ch == '('){
        get(s);
        float result = expression(s);
        get(s);
        return result;
    }
	// T -> -T
    else if(ch == '-'){
        get(s);
        return -term(s); 
    }
	// T -> number | number.dec_number
    else if(ch >= '0' && ch <= '9'){
        float result = number(s);
        ch = peek(s);
        if(ch == '.'){
            get(s);
            result += dec_number(s);
        }
        return result;
    }

    return 0;
}

float expression(char **s){
    float result = term(s);

    char ch = peek(s);
	// E -> T+T | T-T | T*T | T/T
    while(ch == '+' || ch == '-' || ch == '*' || ch == '/'){
        get(s);
        if(ch == '+'){
            result += term(s);
        }
        else if(ch == '-'){
            result -= term(s);
        }
        else if(ch == '*'){
            result *= term(s);
        }
        else{
            result /= term(s);
        }

        ch = peek(s);
    }

    return result;
}