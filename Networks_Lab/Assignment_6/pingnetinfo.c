#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

int isValidIPAddress(char *IPAddr)
{
    int parts = 0;
    int chars = 0;
    int value = 0;

    while (*IPAddr != '\0')
    {
        if (*IPAddr == '.')
        {
            if (chars == 0)
                return 0;

            parts++;
            if (parts == 4)
                return 0;

            chars = 0;
            value = 0;
            IPAddr++;
            continue;
        }

        if ((*IPAddr < '0') || (*IPAddr > '9'))
            return 0;

        value = value * 10 + (*IPAddr - '0');
        if (value > 255)
            return 0;

        chars++;
        IPAddr++;
    }
}

char* reverse_dns_lookup(const char* ip_address) {
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

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        printf("Usage: ./program <host> <n> <T>\n");
        exit(0);
    }

    char ip_address[16];

    if (!isValidIPAddress(argv[1]))
    {
        struct hostent *he;
        struct in_addr **addr_list;

        he = gethostbyname(argv[1]);
        if (he == NULL)
        {
            perror("gethostbyname failed");
            exit(0);
        }

        addr_list = (struct in_addr **)he->h_addr_list;
        if (addr_list[0] == NULL)
        {
            perror("No Address with the given hostname");
            exit(0);
        }

        if (inet_ntop(AF_INET, addr_list[0], ip_address, INET_ADDRSTRLEN) == NULL)
        {
            perror("inet_ntop failed");
            exit(0);
        }
    }
    else
    {
        strcpy(ip_address, argv[1]);
    }

    printf("IP address of %s is %s\n", argv[1], ip_address);

    char *newIp = "142.250.182.238";
    printf("Hostname of %s: %s\n", newIp, reverse_dns_lookup(newIp));
}