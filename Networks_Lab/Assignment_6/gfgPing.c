#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#define PING_PKT_SIZE 64

#define PORT_NO 0

#define PING_SLEEP_RATE 1000000 x

#define RECV_TIMEOUT 1

#define MAX_HOST_LEN 1025

int pingloop = 1;

struct ping_pkt {
    struct icmphdr hdr;
    char msg[PING_PKT_SIZE - sizeof(struct icmphdr)];
};

unsigned short checksum(void *b, int len){
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for(sum = 0; len > 1;len -= 2){
        sum += *buf++;
        if(len == 1) sum += *(unsigned char*)buf;
        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);
        result = ~sum;
        return result;
    }
}

void intHandler(int sig){
    // signal(sig, intHandler);
    pingloop = 0;
}

char *getAddr(char *hostname, struct sockaddr_in *addr_con){
    printf("\nResolving host name..\n");
    struct hostent *he;
    struct in_addr **addr_list;
    char *ip = (char *)malloc(MAX_HOST_LEN * sizeof(char));

    he = gethostbyname(hostname);
    if (he == NULL) return NULL;

    addr_list = (struct in_addr **)he->h_addr_list;
    if (addr_list[0] == NULL)
    {
        perror("No Address with the given hostname");
        return NULL;
    }

    if (inet_ntop(AF_INET, addr_list[0], ip, INET_ADDRSTRLEN) == NULL)
    {
        perror("inet_ntop failed");
        return NULL;
    }

    (*addr_con).sin_family = he->h_addrtype;
    (*addr_con).sin_port = htons(PORT_NO);
    (*addr_con).sin_addr.s_addr = *(long*)he->h_addr_list[0];

    return ip;
}

char *getName(char *ip_address){
    struct in_addr addr;
    struct hostent* host_info;

    // Convert the IP address string to a network address structure
    if (inet_aton(ip_address, &addr) == 0) {
        perror("Error converting IP address to network address structure");
        return NULL; 
    }

    // Perform a reverse DNS lookup to get the host information for the IP address
    host_info = gethostbyaddr((const void *)&addr, sizeof(addr), AF_INET);

    if (host_info == NULL) {
        herror("Error resolving IP address");
        return NULL;
    }

    char *name = (char *)malloc(strlen(host_info->h_name) + 1);
    strcpy(name, host_info->h_name);

    return name;
}

void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *ping_hostname, char *ping_ip, char *rev_host){
    int i, addr_len;
    int ttl_val = 64, msg_count = 0, flag = 1;

    struct ping_pkt packet;
    struct sockaddr_in r_addr;
    struct timespec time_start, time_end, tfs, tfe;
    long double rtt_msec = 0, total_msec = 0;
    struct timeval tv_out;
    tv_out.tv_sec = RECV_TIMEOUT;
    tv_out.tv_usec = 0;

    clock_gettime(CLOCK_MONOTONIC, &tfs);
}

int main(){
    return 0;
}