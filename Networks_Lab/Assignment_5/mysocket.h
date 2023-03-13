//
// Created by chkri on 13-03-2023.
//

#ifndef ASS5_MYSOCKET_H
#define ASS5_MYSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define SOCK_MyTCP 100

int socket_exists = 0;
int my_fd = -1;

int my_socket(int domain, int type, int protocol);

int my_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int my_listen(int sockfd, int backlog);

int my_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int my_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

ssize_t my_send(int sockfd, const void *buf, size_t len);

ssize_t my_recv(int sockfd, void *buf, size_t len);

int my_close(int fd);


#endif //ASS5_MYSOCKET_H
