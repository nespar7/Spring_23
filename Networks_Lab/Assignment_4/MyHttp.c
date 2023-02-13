#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 50


#define BUF_SIZE 50


const char *get_response = "HTTP/1.1 200 OK\r\n"
                           "Expires: %s\r\n"
                           "Cache-Control: no-store\r\n"
                           "Last-Modified: %s\r\n"
                           "Content-length: %ld\r\n"
                           "Content-type: %s\r\n"
                           "Content-language: en-us\r\n\r\n";

const char *put_response = "HTTP/1.1 200 OK\r\n"
                           "Content-language: en-us\r\n"
                           "Content-length: 0\r\n"
                           "Content-type: text/html\r\n\r\n";

const char *response_400 = "HTTP/1.1 400 BAD REQUEST\r\n"
                           "Content-language: en-us\r\n"
                           "Content-length: 0\r\n"
                           "Content-type: text/html\r\n\r\n";

const char *response_403 = "HTTP/1.1 403 FORBIDDEN\r\n"
                           "Content-language: en-us\r\n"
                           "Content-length: 0\r\n"
                           "Content-type: text/html\r\n\r\n";

const char *response_404 = "HTTP/1.1 404 NOT FOUND\r\n"
                           "Content-language: en-us\r\n"
                           "Content-length: 0\r\n"
                           "Content-type: text/html\r\n\r\n";

enum header_type {
    HOST, CONNECTION, DATE, ACCEPT, ACCEPT_LANGUAGE, IF_MODIFIED_SINCE, CONTENT_LANGUAGE, CONTENT_LENGTH, CONTENT_TYPE
};

struct header {
    char *name;
    char *value;
};
typedef struct header header;

header REQ_HEADERS[9];

void toUpper(char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = (char) (str[i] - 'a' + 'A');
        }
    }
}

int receiveFile(int sock_fd, char *file_name, long size) {
    printf("Receiving file: %s\n", file_name);
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    char *buf = (char *) malloc(BUF_SIZE * sizeof(char));
    for (int i = 0; i < BUF_SIZE; i++) {
        buf[i] = '\0';
    }
    while (size > 0) {
        int buf_len = recv(sock_fd, buf, BUF_SIZE, 0);
        if (buf_len < 1)break;
        fwrite(buf, sizeof(char), buf_len, file);
        size -= buf_len;
    }
    free(buf);
    fclose(file);
    return 0;
}

int sendFile(int sock_fd, char *file_name, long size) {
    printf("Sending file: %s\n", file_name);
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    char *buf = (char *) malloc(BUF_SIZE * sizeof(char));
    while (size > 0) {
        int buf_len = fread(buf, sizeof(char), BUF_SIZE, file);
        if (buf_len < 1)break;
        send(sock_fd, buf, buf_len, 0);
        size -= buf_len;
    }
    fclose(file);
    free(buf);
    return 0;
}

void sendToSocket(int sock_fd, const char *msg) {
    while (strlen(msg) >= BUF_SIZE) {
        send(sock_fd, msg, BUF_SIZE, 0);
        msg += BUF_SIZE;
    }
    send(sock_fd, msg, strlen(msg), 0);
}

char *readline(int fd, int *size) {
    char c;
    *size = 0;
    int bufferSize = BUFFER_SIZE;
    char *buffer = (char *) malloc(sizeof(char) * bufferSize);
    while (1) {
        int i = (int) read(fd, &c, 1);
        if (i <= 0) {
            return NULL;
        }
        buffer[*size] = c;
        *size = *size + 1;
        if (*size >= BUFFER_SIZE) {
            buffer = (char *) realloc(buffer, sizeof(char) * (bufferSize + BUFFER_SIZE));
            bufferSize = bufferSize + BUFFER_SIZE;
        }
        if (c == '\r') {
            (int) read(fd, &c, 1);
            break;
        }
        if (c == '\n') {
            break;
        }
    }
    *size = *size - 1;
    buffer[*size] = '\0';
    return buffer;
}


int main() {
    int socket_fd, new_socket_fd;
    struct sockaddr_in client_addr, server_addr;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket\n");
        exit(0);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Unable to bind local address\n");
        exit(0);
    }
    if (listen(socket_fd, 5) < 0) {
        perror("listen error");
        exit(0);
    }
    while (1) {
        int addr_len = sizeof(client_addr);
        if ((new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, (socklen_t *) &addr_len)) < 0) {
            perror("accept error");
            exit(0);
        }
        if (fork() == 0) {
            printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            close(socket_fd);
            handle_request:; // handle request
            int req_len;
            char *req;
            req = readline(new_socket_fd, &req_len);
            if (req == NULL) {
                perror("read error");
                exit(0);
            }
            char *method = strtok(req, " ");
            char *url = strtok(NULL, " ");
            while (1) {
                req = readline(new_socket_fd, &req_len);
                if (req == NULL) {
                    perror("read error");
                    exit(0);
                }
                if (req_len == 0) {
                    break;
                }
                char *name = req;
                char *value = strchr(req, ':');
                *value = '\0';
                value++;
                while (*value == ' ' || *value == '\t') {
                    value++;
                }
                toUpper(name);
                printf("%s: %s\n", name, value);
                if (strcmp(name, "HOST") == 0) {
                    REQ_HEADERS[HOST].name = name;
                    REQ_HEADERS[HOST].value = value;
                } else if (strcmp(name, "CONNECTION") == 0) {
                    REQ_HEADERS[CONNECTION].name = name;
                    REQ_HEADERS[CONNECTION].value = value;
                } else if (strcmp(name, "DATE") == 0) {
                    REQ_HEADERS[DATE].name = name;
                    REQ_HEADERS[DATE].value = value;
                } else if (strcmp(name, "ACCEPT") == 0) {
                    REQ_HEADERS[ACCEPT].name = name;
                    REQ_HEADERS[ACCEPT].value = value;
                } else if (strcmp(name, "ACCEPT-LANGUAGE") == 0) {
                    REQ_HEADERS[ACCEPT_LANGUAGE].name = name;
                    REQ_HEADERS[ACCEPT_LANGUAGE].value = value;
                } else if (strcmp(name, "IF-MODIFIED-SINCE") == 0) {
                    REQ_HEADERS[IF_MODIFIED_SINCE].name = name;
                    REQ_HEADERS[IF_MODIFIED_SINCE].value = value;
                } else if (strcmp(name, "CONTENT-LANGUAGE") == 0) {
                    REQ_HEADERS[CONTENT_LANGUAGE].name = name;
                    REQ_HEADERS[CONTENT_LANGUAGE].value = value;
                } else if (strcmp(name, "CONTENT-LENGTH") == 0) {
                    REQ_HEADERS[CONTENT_LENGTH].name = name;
                    REQ_HEADERS[CONTENT_LENGTH].value = value;
                } else if (strcmp(name, "CONTENT-TYPE") == 0) {
                    REQ_HEADERS[CONTENT_TYPE].name = name;
                    REQ_HEADERS[CONTENT_TYPE].value = value;
                }
            }
            printf("%s\n", REQ_HEADERS[ACCEPT].value);
            char response[1024];
            if (strcmp(method, "GET") == 0) {
                url++;
                printf("URL: %s\n", url);
                FILE *fp = fopen(url, "r");
                if (fp == NULL) {
                    sendToSocket(new_socket_fd, response_404);
                    break;
                }
                fseek(fp, 0, SEEK_END);
                long file_size = ftell(fp);
                fclose(fp);
                struct stat fileInfo;
                char LastModified[80];
                if (stat(url, &fileInfo) == 0) {
                    struct tm *timeInfo = gmtime(&fileInfo.st_mtime);
                    strftime(LastModified, 80, "%a, %d %b %Y %T GMT", timeInfo);
                    printf("Last modified time: %s\n", LastModified);
                } else {
                    printf("Error getting file information\n");
                }

                time_t t;
                t = time(NULL);
                struct tm *threedayslater;
                threedayslater = gmtime(&t);
                threedayslater->tm_mday += 3;
                mktime(threedayslater);

                char ThreeDaysLaterButItsAString[80];
                strftime(ThreeDaysLaterButItsAString, 80, "%a, %d %b %Y %T GMT", threedayslater);

                sprintf(response, get_response, ThreeDaysLaterButItsAString, LastModified, file_size,
                        REQ_HEADERS[ACCEPT].value);
                printf("response:\n%s", response);
                sendToSocket(new_socket_fd, response);
                sendFile(new_socket_fd, url, file_size);

            } else if (strcmp(method, "PUT") == 0) {
                printf("url:%s\n", url);
                url++;
                printf("url:%s\n", url);
                printf("content-length:%s\n", REQ_HEADERS[CONTENT_LENGTH].value);
                int status = receiveFile(new_socket_fd, url, atoi(REQ_HEADERS[CONTENT_LENGTH].value));
                if (status == -1) {
                    strcpy(response, response_403);
                    printf("response:%s", response);
                    sendToSocket(new_socket_fd, response);
                    break;
                }
                strcpy(response, put_response);
                printf("response:%s", response);
                sendToSocket(new_socket_fd, response);
            }

            if (strcmp(REQ_HEADERS[CONNECTION].value, "keep-alive") == 0) {
                goto handle_request;
            }
            for (int i = 0; i < 9; ++i) {
                free(REQ_HEADERS[i].name);
            }
            return 0;
        }
        close(new_socket_fd);
    }

}
