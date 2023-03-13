//
// Created by chkri on 13-03-2023.
//

#include "mysocket.h"

#define MAX 10

struct MESSAGE_TABLE {
    void *Messages[MAX];
    size_t lengths[MAX];
    int front;
    int rear;
    int itemCount;
};

struct MESSAGE_TABLE Received_Message = {
        .front = 0,
        .rear = -1,
        .itemCount = 0
};
struct MESSAGE_TABLE Send_Message = {
        .front = 0,
        .rear = -1,
        .itemCount = 0
};

int isEmpty(struct MESSAGE_TABLE messages) {
    return messages.itemCount == 0;
}

int isFull(struct MESSAGE_TABLE messages) {
    return messages.itemCount == MAX;
}

int size(struct MESSAGE_TABLE messages) {
    return messages.itemCount;
}

size_t insert(struct MESSAGE_TABLE *messages, const void *data, size_t len) {

    if (!isFull(*messages)) {

        if (messages->rear == MAX - 1) {
            messages->rear = -1;
        }
        messages->rear++;
        messages->Messages[messages->rear] = malloc(len);
        memcpy(messages->Messages[messages->rear], data, len);
        messages->lengths[messages->rear] = len;
        messages->itemCount++;
        return len;
    }
    return -1;
}

void removeData(struct MESSAGE_TABLE *messages) {

    if (!isEmpty(*messages)) {
        messages->lengths[messages->front] = 0;
        free(messages->Messages[messages->front++]);
        if (messages->front == MAX) {
            messages->front = 0;
        }
        messages->itemCount--;
    }
}

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

void send_thread() {

}

void recv_thread() {

}

int my_socket(int domain, int type, int protocol) {
    if (!socket_exists && type == SOCK_MyTCP) {
        type = SOCK_STREAM;
    } else return -1;
    my_fd = socket(domain, type, protocol);
    if (my_fd == -1) {
        return -1;
    } else {
        socket_exists = 1;
        return my_fd;
    }
}

int my_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (sockfd != my_fd) return -1;
    return bind(sockfd, addr, addrlen);
}

int my_listen(int sockfd, int backlog) {
    if (sockfd != my_fd) return -1;
    return listen(sockfd, backlog);
}

int my_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if (sockfd != my_fd) return -1;
    send_thread();
    recv_thread();
    return accept(sockfd, addr, addrlen);
}

int my_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (sockfd != my_fd) return -1;
    return connect(sockfd, addr, addrlen);
}

int my_close(int fd) {
    if (fd != my_fd) return -1;
    if (!isEmpty(Send_Message) || !isEmpty(Received_Message)) {
        //wait
    }
    if (socket_exists) {
        Received_Message.front = 0;
        Received_Message.rear = -1;
        Received_Message.itemCount = 0;
        Send_Message.front = 0;
        Send_Message.rear = -1;
        Send_Message.itemCount = 0;
        memset(Received_Message.lengths, 0, MAX * sizeof(size_t));
        memset(Send_Message.lengths, 0, MAX * sizeof(size_t));

        return close(fd);
    }
    return -1;
}

ssize_t my_send(int sockfd, const void *buf, size_t len) {
    if (sockfd != my_fd) return -1;
    while (isFull(Send_Message)) {
        //wait
    }
    return (ssize_t) insert(&Send_Message, buf, len);
}

ssize_t my_recv(int sockfd, void *buf, size_t len) {
    if (sockfd != my_fd) return -1;
    while (isEmpty(Received_Message)) {
        //wait
    }
    ssize_t recv_len = (ssize_t) min(Received_Message.lengths[Received_Message.front], len);
    memcpy(buf, Received_Message.Messages[Received_Message.front], recv_len);
    removeData(&Received_Message);
    return recv_len;

}