#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/wait.h>

#define BUFFMAX 100

const char *get_request = "GET %s HTTP/1.1\r\n"
                          "Host: %s:%d\r\n"
                          "Connection: close\r\n"
                          "Date: %s\r\n"
                          "Accept: %s\r\n"
                          "Accept-Language: en-us\r\n"
                          "If-Modified-Since: %s\r\n\r\n";

const char *put_request = "PUT %s/%s HTTP/1.1\r\n"
                          "Host: %s:%d\r\n"
                          "Connection: close\r\n"
                          "Date: %s\r\n"
                          "Accept: %s\r\n"
                          "Accept-Language: en-us\r\n"
                          "Content-language: en-us\r\n"
                          "Content-length: %ld\r\n"
                          "Content-type: %s\r\n\r\n";


enum header_type {
    EXPIRES, CACHE_CONTROL, LAST_MODIFIED, CONTENT_LANGUAGE, CONTENT_LENGTH, CONTENT_TYPE
};

struct header {
    char *name;
    char *value;
};
typedef struct header header;

header RES_HEADERS[6];

void toUpper(char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = (char) (str[i] - 'a' + 'A');
        }
    }
}

int minimum(int x) {
    return x < BUFFMAX ? x : BUFFMAX;
}

char *readline(int fd, int *size) {
    char c;
    *size = 0;
    int bufferSize = BUFFMAX;
    char *buffer = (char *) malloc(sizeof(char) * bufferSize);
    while (1) {
        int i = (int) read(fd, &c, 1);
        if (i <= 0) {
            return NULL;
        }
        buffer[*size] = c;
        *size = *size + 1;
        if (*size >= BUFFMAX) {
            buffer = (char *) realloc(buffer, sizeof(char) * (bufferSize + BUFFMAX));
            bufferSize = bufferSize + BUFFMAX;
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

// function to send get request
int send_data(int sockfd, char *buffer, int buffsize) {
    const char *temp = (const char *) buffer;

    // while buffer still has data, send 50 or buffsize(minimum of) number of bytes
    while (buffsize > 0) {
        int response = send(sockfd, temp, minimum(buffsize), 0);
        if (response < 0) return -1;

        // increment the temp string to the position of the next character to be sent
        temp += response;
        buffsize -= response;
    }
    return 0;
}

int send_file(int sockfd, FILE *fp) {
    char buffer[BUFFMAX];
    int response;

    int readsize;
    readsize = fread(buffer, sizeof(char), BUFFMAX, fp);
    while (readsize) {
        response = send_data(sockfd, buffer, readsize);
        if (response < 0) return -1;
        readsize = fread(buffer, sizeof(char), BUFFMAX, fp);
    }

    return 0;
}

int receive_file(int sock_fd, char *file_name, long size) {
    printf("Receiving file: %s\n", file_name);
    FILE *file = fopen(file_name, "w");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    char *buf = (char *) malloc(BUFFMAX * sizeof(char));
    for (int i = 0; i < BUFFMAX; i++) {
        buf[i] = '\0';
    }
    while (size > 0) {
        int buf_len = recv(sock_fd, buf, BUFFMAX, 0);
        if (buf_len < 1)break;
        fwrite(buf, sizeof(char), buf_len, file);
        size -= buf_len;
    }
    free(buf);
    fclose(file);
    return 0;
}

// function to send put request
int send_put_request(int sockfd, char *buffer, int buffsize) {
    const char *temp = (const char *) buffer;
    int response;

    // while buffer still has data, send 50 or buffsize(minimum of) number of bytes
    while (buffsize > 0) {
        response = send(sockfd, temp, minimum(buffsize), 0);
        if (response < 0) return -1;

        // increment the temp string to the position of the next character to be sent
        temp += response;
        buffsize -= response;
    }

    return 0;
}

char *takeInput(FILE *fp, size_t size) {
    char *str;
    int ch;
    size_t len = 0;

    str = realloc(NULL, sizeof(char) * size);
    if (!str)
        return str;
    ch = fgetc(fp);
    int initial_spaces_done = 0;
    while (EOF != ch && ch != '\n') {
        if (!initial_spaces_done) {
            if (ch == ' ') {
                ch = fgetc(fp);
                continue;
            } else initial_spaces_done = 1;
        }
        str[len++] = ch;
        if (len == size) {
            size += 16;
            str = realloc(str, sizeof(char) * (size));
            if (!str)
                return str;
        }

        ch = fgetc(fp);
    }
    str[len++] = '\0';

    return realloc(str, sizeof(char) * len);
}

char *separate(char *prompt, char *host, char *file, char *directory, int *port) {
    char *method1 = strtok(prompt, " ");
    if (strcmp(method1, "QUIT") == 0)
        return file;
    char *url = strtok(NULL, " ");
    int a, b, c, d;
    sscanf(url, "http://%d.%d.%d.%d%s", &a, &b, &c, &d, directory);
    sprintf(host, "%d.%d.%d.%d", a, b, c, d);
    if (strchr(directory, ':') != NULL) {
        char *temp = strchr(directory, ':');
        temp++;
        *port = atoi(temp);
        directory[strlen(directory) - strlen(temp) - 1] = '\0';
    } else {
        *port = 80;
    }

    if (directory[strlen(directory) - 1] == '/')
        directory[strlen(directory) - 1] = '\0';

    if (strcmp(method1, "PUT") == 0) {
        file = strtok(NULL, " ");
    }else if (strcmp(method1, "GET") == 0) {
        file = directory + strlen(directory) - 1;
        while (*file != '/') {
            file--;
        }
        file++;
    }

    return file;
}

int main() {
    while (1) {
        printf("MyOwnBrowser> ");
        char *prompt = takeInput(stdin, 50);
        char host[16];
        char *file;
        char directory[100];
        int port;
        file = separate(prompt, host, file, directory, &port);

        int response;
        if (!strcmp(prompt, "QUIT")) {
            printf("Bye have a nice day ;)");
            break;
        }

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        inet_aton(host, &servaddr.sin_addr);

        printf("%s:%d\n", host, port);
        response = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (response < 0) {
            perror("Connection error");
            continue;
        }
        printf("%s:%d\n", host, port);

        char request[1024];
        if (!strcmp(prompt, "GET")) {
            char *last_slash = strrchr(directory, '/');
            char *extension = strrchr(last_slash ? last_slash : directory, '.');
            extension = extension + 1;

            char Accept[20];
            if (!strcmp(extension, "pdf")) {
                sprintf(Accept, "application/pdf");
            } else if (!strcmp(extension, "jpg")) {
                sprintf(Accept, "image/jpeg");
            } else if (!strcmp(extension, "html")) {
                sprintf(Accept, "text/html");
            } else {
                sprintf(Accept, "text/*");
            }

            time_t t;
            t = time(NULL);
            struct tm *rightnow;
            rightnow = gmtime(&t);
            struct tm *twodaysago;
            twodaysago = rightnow;
            twodaysago->tm_mday -= 2;
            mktime(rightnow);
            mktime(twodaysago);

            char RightNowButItsAString[80];
            char TwoDaysAgoButItsAString[80];
            strftime(RightNowButItsAString, 80, "%a, %d %b %Y %T GMT", rightnow);
            strftime(TwoDaysAgoButItsAString, 80, "%a, %d %b %Y %T GMT", twodaysago);

            sprintf(request, get_request, directory, host, port, RightNowButItsAString, Accept,
                    TwoDaysAgoButItsAString);
            printf("%s\n", request);

            send_data(sockfd, request, strlen(request) + 1);
        } else if (!strcmp(prompt, "PUT")) {
            char *extension = strrchr(file, '.');
            extension = extension + 1;

            char Accept[20];
            if (!strcmp(extension, "pdf")) {
                sprintf(Accept, "application/pdf");
            } else if (!strcmp(extension, "jpg")) {
                sprintf(Accept, "image/jpeg");
            } else if (!strcmp(extension, "html")) {
                sprintf(Accept, "text/html");
            } else {
                sprintf(Accept, "text/*");
            }

            char *full_path;

            FILE *fp = fopen(file, "rb");
            if (fp == NULL) {
                printf("Could not open file %s\n", full_path);
                continue;
            }
            fseek(fp, 0, SEEK_END);
            long file_size = ftell(fp);
            rewind(fp);

            time_t t;
            t = time(NULL);
            struct tm *rightnow;
            rightnow = gmtime(&t);

            char RightNowButItsAString[80];
            strftime(RightNowButItsAString, 80, "%a, %d %b %Y %T GMT", rightnow);

            sprintf(request, put_request, directory, file, host, port, RightNowButItsAString, Accept, file_size,
                    Accept);
            printf("%s\n", request);

            response = send_put_request(sockfd, request, strlen(request));
            if (response < 0) {
                perror("Cannot send put request headers");
                exit(1);
            }

            response = send_file(sockfd, fp);
            if (response < 0) {
                perror("Cannot send file");
                exit(1);
            }

            fclose(fp);
        }
        int req_len;
        char *req;
        req = readline(sockfd, &req_len);
        char *version = strtok(req, " ");
        char *statusCode = strtok(NULL, " ");
        char *statusMessage = strtok(NULL, " ");
        while (1) {
            req = readline(sockfd, &req_len);
            if (req == NULL) {
                perror("read error");
                exit(0);
            }
            if (req_len == 0) {
                break;
            }

            char *name = req;
            while (name[0] == ' ' || name[0] == '\t') {
                name++;
            }
            char *value = strchr(req, ':');
            *value = '\0';
            value++;
            while (*value == ' ' || *value == '\t') {
                value++;
            }
            toUpper(name);
            if (!strcmp(name, "EXPIRES")) {
                RES_HEADERS[EXPIRES].name = name;
                RES_HEADERS[EXPIRES].value = value;
            } else if (!strcmp(name, "LAST-MODIFIED")) {
                RES_HEADERS[LAST_MODIFIED].name = name;
                RES_HEADERS[LAST_MODIFIED].value = value;
            } else if (strcmp(name, "CACHE-CONTROL") == 0) {
                RES_HEADERS[CACHE_CONTROL].name = name;
                RES_HEADERS[CACHE_CONTROL].value = value;
            } else if (strcmp(name, "CONTENT-LANGUAGE") == 0) {
                RES_HEADERS[CONTENT_LANGUAGE].name = name;
                RES_HEADERS[CONTENT_LANGUAGE].value = value;
            } else if (strcmp(name, "CONTENT-LENGTH") == 0) {
                RES_HEADERS[CONTENT_LENGTH].name = name;
                RES_HEADERS[CONTENT_LENGTH].value = value;
            } else if (strcmp(name, "CONTENT-TYPE") == 0) {
                RES_HEADERS[CONTENT_TYPE].name = name;
                RES_HEADERS[CONTENT_TYPE].value = value;
                printf("content type %s\n", value);
            } else {
                printf("Nice %s header you got there, but no thanks\n", name);
            }
        }
        printf("HTTP/%s %s %s\n", version, statusCode, statusMessage);
        if (!strcmp(statusCode, "200")) {
            long content_length = atol(RES_HEADERS[CONTENT_LENGTH].value);
            printf("Content-Length: %ld", content_length);
            if (content_length) {
                int child = fork();
                if(child == 0){
                    receive_file(sockfd, file, content_length);
                    if (!strcmp(RES_HEADERS[CONTENT_TYPE].value, "text/html")) {
                        char *mozilla = "firefox";
                        char *browser_path = NULL;
                        char *path = getenv("PATH");
                        char *path_part = strtok(path, ":");
                        while (path_part != NULL) {
                            char executable[4096];
                            sprintf(executable, "%s/%s", path_part, mozilla);
                            if (access(executable, F_OK) == 0) {
                                browser_path = executable;
                                break;
                            }
                            path_part = strtok(NULL, ":");
                        }
                        if (browser_path == NULL) {
                            char *google = "google-chrome";
                            browser_path = NULL;
                            path = getenv("PATH");
                            path_part = strtok(path, ":");
                            while (path_part != NULL) {
                                char exec[4096];
                                sprintf(exec, "%s/%s", path_part, google);
                                if (access(exec, F_OK) == 0) {
                                    browser_path = exec;
                                    break;
                                }
                                path_part = strtok(NULL, ":");
                            }
                            if (browser_path == NULL) {
                                perror("No browsers found");
                                continue;
                            }
                        }

                        char *const args[] = {browser_path, file, NULL};
                        execvp(browser_path, args);
                    } else if (!strcmp(RES_HEADERS[CONTENT_TYPE].value, "application/pdf")) {
                        char *adobe = "acroread";
                        char *reader_path = NULL;
                        char *path = getenv("PATH");
                        char *path_part = strtok(path, ":");
                        while (path_part != NULL) {
                            char executable[4096];
                            sprintf(executable, "%s/%s", path_part, adobe);
                            if (access(executable, F_OK) == 0) {
                                reader_path = executable;
                                break;
                            }
                            path_part = strtok(NULL, ":");
                        }
                        if (reader_path == NULL) {
                            char *evince = "evince";
                            path = getenv("PATH");
                            path_part = strtok(path, ":");
                            while (path_part != NULL) {
                                char executable[4096];
                                sprintf(executable, "%s/%s", path_part, evince);
                                if (access(executable, F_OK) == 0) {
                                    reader_path = executable;
                                    break;
                                }
                                path_part = strtok(NULL, ":");
                            }
                            if(reader_path == NULL){
                                perror("No browsers found");
                                continue;
                            }
                        }

                        char *const args[] = {reader_path, file, NULL};
                        execvp(reader_path, args);
                    } else if (!strcmp(RES_HEADERS[CONTENT_TYPE].value, "image/jpeg")) {
                        char *eog = "eog";
                        char *eog_path = NULL;
                        char *path = getenv("PATH");
                        char *path_part = strtok(path, ":");
                        while (path_part != NULL) {
                            char executable[4096];
                            sprintf(executable, "%s/%s", path_part, eog);
                            if (access(executable, F_OK) == 0) {
                                eog_path = executable;
                                break;
                            }
                            path_part = strtok(NULL, ":");
                        }
                        if (eog_path == NULL) {
                            perror("Image Viewer NOT found");
                            continue;
                        }
                        char *const args[] = {eog_path, file, NULL};
                        execvp(eog_path, args);
                    } else if (!strcmp(RES_HEADERS[CONTENT_TYPE].value, "text/*")) {
                        char *gedit = "gedit";
                        char *editor_path = NULL;
                        char *path = getenv("PATH");
                        char *path_part = strtok(path, ":");
                        while (path_part != NULL) {
                            char executable[4096];
                            sprintf(executable, "%s/%s", path_part, gedit);
                            if (access(executable, F_OK) == 0) {
                                editor_path = executable;
                                break;
                            }
                            path_part = strtok(NULL, ":");
                        }
                        if (editor_path == NULL) {
                            perror("Gedit NOT found");
                            continue;
                        }
                        char *const args[] = {editor_path, file, NULL};
                        execvp(editor_path, args);
                    } else {
                        printf("Ahh I wish I could have seen how beautiful the %s file was. But alas I don't want to\n", RES_HEADERS[CONTENT_TYPE].value);
                    }
                }
                else{
                    continue;
                }
            } else {
                printf("Successfully updated %s\n", file);
            }
        } else if (!strcmp(statusCode, "400")) {
            printf("%s\n", statusMessage);
        } else if (!strcmp(statusCode, "403")) {
            printf("%s\n", statusMessage);
        } else if (!strcmp(statusCode, "404")) {
            printf("%s\n", statusMessage);
        } else {
            printf("We don't do that here\n");
        }
    }
}
