#ifndef ASS5_MY_SOCKET_H
#define ASS5_MY_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define SOCK_MyTCP 100


int my_socket(int domain, int type, int protocol);

int my_bind(int fd, const struct sockaddr *addr, socklen_t len);

int my_listen(int fd, int n);

int my_accept(int fd, struct sockaddr *addr, socklen_t *len);

int my_connect(int fd, const struct sockaddr *addr, socklen_t len);

ssize_t my_send(int fd, const void *buf, size_t len);

ssize_t my_recv(int fd, void *buf, size_t len);

int my_close(int fd);


#endif //ASS5_MY_SOCKET_H
