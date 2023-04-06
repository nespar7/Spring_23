#include "mysocket.h"

#define MAX_MESSAGE_TABLE_SIZE 10
#define MAX_BUFF_SIZE 1000

#define SLEEP_TIME 1


int fd_exists = 0;
int fd2_exists = 0;
int my_fd = -1;
int my_fd2 = -1;
int is_server = 0;
int connection_closed = 1;


pthread_t S;
pthread_t R;

struct MESSAGE_TABLE {
    void *Messages[MAX_MESSAGE_TABLE_SIZE];
    size_t lengths[MAX_MESSAGE_TABLE_SIZE];
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
    return messages.itemCount == MAX_MESSAGE_TABLE_SIZE;
}

size_t insert(struct MESSAGE_TABLE *messages, const void *data, size_t len) {

    if (!isFull(*messages)) {

        if (messages->rear == MAX_MESSAGE_TABLE_SIZE - 1) {
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
        if (messages->front == MAX_MESSAGE_TABLE_SIZE) {
            messages->front = 0;
        }
        messages->itemCount--;
    }
}

size_t min(size_t a, size_t b) {
    return a < b ? a : b;
}

void send_thread();

void receive_thread();

int my_socket(int domain, int type, int protocol) {
    if (fd_exists)return -1;
    if (type == SOCK_MyTCP) {
        type = SOCK_STREAM;
    } else return -1;
    my_fd = socket(domain, type, protocol);
    if (my_fd < 0) {
        return my_fd;
    } else {
        fd_exists = 1;
        return my_fd;
    }
}

int my_bind(int fd, const struct sockaddr *addr, socklen_t len) {
    if (fd != my_fd) return -1;
    return bind(fd, addr, len);
}

int my_listen(int fd, int n) {
    if (fd != my_fd) return -1;
    int flag = listen(fd, n);
    if (flag < 0)return flag;
    is_server = 1;
    return flag;
}

int my_accept(int fd, struct sockaddr *addr, socklen_t *addr_len) {
    if (!is_server)return -1;
    if (fd != my_fd) return -1;
    if (fd2_exists)return -1;
    int new_fd = accept(fd, addr, addr_len);
    if (new_fd < 0) return new_fd;
    my_fd2 = new_fd;
    fd2_exists = 1;
    connection_closed = 0;
    send_thread();
    receive_thread();
    return my_fd2;
}

int my_connect(int fd, const struct sockaddr *addr, socklen_t len) {
    if (is_server)return -1;
    if (fd != my_fd) return -1;
    int flag = connect(fd, addr, len);
    if (flag < 0) return flag;
    connection_closed = 0;
    send_thread();
    receive_thread();
    return flag;
}

int my_close(int fd) {
    //printf("close\n");
    //printf("send_message: %d, receive_message: %d\n", Send_Message.itemCount, Received_Message.itemCount);
    if (fd == my_fd) {
        if (!fd_exists)return -1;
    } else if (fd == my_fd2) {
        if (!fd2_exists)return -1;
    } else return -1;

    while (!isEmpty(Send_Message) && !connection_closed) {
        //wait
        sleep(SLEEP_TIME);
    }
    int flag = close(fd);
    if (flag < 0)return flag;
    if (fd == my_fd) {
        my_fd = -1;
        fd_exists = 0;
        if (!is_server) {
            connection_closed = 1;
        }
    } else if (fd == my_fd2) {
        my_fd2 = -1;
        fd2_exists = 0;
        connection_closed = 1;
    }
    if (connection_closed) {
        pthread_cancel(S);
        pthread_cancel(R);
        while (!isEmpty(Received_Message)) {
            removeData(&Received_Message);
        }
        while (!isEmpty(Send_Message)) {
            removeData(&Send_Message);
        }
        Received_Message.front = 0;
        Received_Message.rear = -1;
        Received_Message.itemCount = 0;
        Send_Message.front = 0;
        Send_Message.rear = -1;
        Send_Message.itemCount = 0;
        memset(Received_Message.lengths, 0, MAX_MESSAGE_TABLE_SIZE * sizeof(size_t));
        memset(Send_Message.lengths, 0, MAX_MESSAGE_TABLE_SIZE * sizeof(size_t));
    }
    return flag;

}

ssize_t my_send(int fd, const void *buf, size_t len) {
    if (!is_server && fd != my_fd) return -1;
    if (is_server && fd != my_fd2) return -1;
    while (isFull(Send_Message) && !connection_closed) {
        //wait
        sleep(SLEEP_TIME);
    }
    if (connection_closed)return -1;
    //printf("my_send: %d, %d\n", Send_Message.itemCount, Send_Message.lengths[Send_Message.rear]);
    ssize_t flag = (int) insert(&Send_Message, buf, len);
    //printf("my_send: %d, %d\n", Send_Message.itemCount, Send_Message.lengths[Send_Message.rear]);
    return flag;
}

ssize_t my_recv(int fd, void *buf, size_t len) {
    if (!is_server && fd != my_fd) return -1;
    if (is_server && fd != my_fd2) return -1;
    if (isEmpty(Received_Message) && connection_closed)return 0;
    while (isEmpty(Received_Message)) {
        //wait
        sleep(SLEEP_TIME);
    }
    //printf("my_recv: %d, %d\n", Received_Message.itemCount, Received_Message.lengths[Received_Message.front]);
    ssize_t receive_len = (ssize_t) min(Received_Message.lengths[Received_Message.front], len);
    memcpy(buf, Received_Message.Messages[Received_Message.front], receive_len);
    removeData(&Received_Message);
    //printf("my_recv: %d, %d\n", Received_Message.itemCount, Received_Message.lengths[Received_Message.front]);
    return receive_len;

}

/*
 * fd = socket()
 * bind()
 * listen()
 * new_fd=accept(fd)
 * close(fd)
 * fd2= socket()
 * bind()
 * listen()
 * new_fd2=accept(fd2)
 *
 */

// The receiver thread waits on a recv call on the TCP socket, receives data that comes in, and interprets
// it as a message. The message is then stored in the Received_Message table.
void *receiveData() {
    int fd = is_server ? my_fd2 : my_fd;
    // Receive the message until `EOT` character is received
    void *message;
    void *buffer;
    message = malloc(MAX_BUFF_SIZE);
    buffer = malloc(MAX_BUFF_SIZE);

    pthread_cleanup_push(free, message) ;
            pthread_cleanup_push(free, buffer) ;

                    size_t max_len = MAX_BUFF_SIZE;
                    size_t message_len = 0;

                    // Receive the message until `EOT` character is received
                    while (1) {
                        memset(buffer, 0, MAX_BUFF_SIZE);
                        ssize_t receive_len = recv(fd, buffer, MAX_BUFF_SIZE - 1, 0);
                        //printf("receive_len: %d\n", receive_len);
                        if (receive_len < 0) {
                            perror("receive failed");
                            connection_closed = 1;
                            pthread_exit(NULL);
                        }
                        if (receive_len == 0) {
                            connection_closed = 1;
                            pthread_exit(NULL);
                        }
                        char *eot = strchr(buffer, 4);
                        size_t remaining_len = receive_len;
                        while (eot != NULL) {
                            size_t curr_len = strcspn(buffer + receive_len - remaining_len, "\4");
                            remaining_len = remaining_len - curr_len - 1;
                            //printf("message_len: %d, curr_len: %d, remaining_len: %d\n", message_len, curr_len,remaining_len);
                            if (message_len + curr_len > max_len) {
                                max_len += MAX_BUFF_SIZE;
                                message = realloc(message, max_len);
                            }
                            memcpy(message + message_len, buffer + receive_len - remaining_len - curr_len - 1,
                                   curr_len);
                            message_len += curr_len;
                            while (isFull(Received_Message))sleep(SLEEP_TIME);
                            insert(&Received_Message, message, message_len);
                            //printf("%d receiveData: %d, %d\n", message_len, Received_Message.itemCount,Received_Message.lengths[Received_Message.rear]);
                            ((char *) (message))[message_len] = '\0';
                            //printf("Received DATA: %s\n", (char *) message);
                            eot = strchr(eot + 1, 4);
                            free(message);
                            message = malloc(MAX_BUFF_SIZE);

                            message_len = 0;
                            max_len = MAX_BUFF_SIZE;
                        }
                        if (remaining_len > 0) {
                            if (message_len + remaining_len > max_len) {
                                max_len += MAX_BUFF_SIZE;
                                message = realloc(message, max_len);
                            }
                            memcpy(message, buffer + receive_len - remaining_len, remaining_len);
                            message_len += remaining_len;
                        } else {
                            message_len = 0;
                            free(message);
                            message = malloc(MAX_BUFF_SIZE);
                            max_len = MAX_BUFF_SIZE;
                        }

                    }
            pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

}

// The sender thread wait on send call on the TCP socket, sends data that comes in, and interprets
// it as a message. The message is then stored in the Send_Message table.
void *sendData() {
    int fd = is_server ? my_fd2 : my_fd;
    while (1) {
        // Read the first message from the send table and store it in a buffer
        // Send the message until it's length is reached adding `EOT` character at the end
        // Remove the message from the send table

        if (connection_closed)pthread_exit(NULL);

        char *messageStart, *message;
        // Reading the first message from the send table


        // Wait until the send table is not empty
        while (isEmpty(Send_Message)) {
            sleep(SLEEP_TIME);
        }
        // Read the first message from the send table
        size_t message_len = Send_Message.lengths[Send_Message.front] + 1;
        messageStart = malloc(message_len);
        pthread_cleanup_push(free, messageStart) ;
                message = messageStart;
                memcpy(message, Send_Message.Messages[Send_Message.front], message_len - 1);
                message[message_len - 1] = 4;
                removeData(&Send_Message);
                //printf("sendData: %d\n", message_len);
                // Sending the message until it's length is reached adding `EOT` character at the end
                while (message_len > MAX_BUFF_SIZE && !connection_closed) {
                    ssize_t flag = send(fd, message, MAX_BUFF_SIZE, 0);
                    //printf("send_len: %d\n", flag);
                    if (flag < 0) {
                        perror("Error in sending data\n");
                        connection_closed = 1;
                        pthread_exit(NULL);
                    }
                    message_len -= flag;
                    message += flag;
                }
                while (message_len > 0 && !connection_closed) {
                    ssize_t flag = send(fd, message, message_len, 0);
                    //printf("send_len: %d\n", flag);
                    if (flag < 0) {
                        perror("Error in sending data\n");
                        connection_closed = 1;
                        pthread_exit(NULL);
                    }
                    message_len -= flag;
                    message += flag;
                }
        pthread_cleanup_pop(1);
    }
}


void send_thread() {
    // spawn a sendData thread named S
    pthread_create(&S, NULL, sendData, NULL);
}

void receive_thread() {
    // spawn a receiveData thread named R
    pthread_create(&R, NULL, receiveData, NULL);
}