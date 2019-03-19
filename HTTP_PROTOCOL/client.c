#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define USAGE_STR "Usage: %s domain_address"
#define SERVICE_STR "http"

int main(int argc, char** argv) {
    if(argc != 2) {
        printf(USAGE_STR, argv[0]);
        exit(EXIT_FAILURE);
    }

    struct addrinfo* addr_list;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM; // For TCP protocol

    int status = getaddrinfo(argv[1], SERVICE_STR, &hints, &addr_list);
    if(status) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    for(struct addrinfo* temp = addr_list; temp != NULL; temp = temp->ai_next) {
        void* addr = NULL;
        char* ipver = NULL;

        switch(temp->ai_family) {
            case AF_INET: {
                struct sockaddr_in* ipv4 = (struct sockaddr_in*)temp->ai_addr;
                addr = (struct in_addr*)&ipv4->sin_addr;
                ipver = "IPv4";
            }
            break;
            case AF_INET6: {
                struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)temp->ai_addr;
                addr = (struct in6_addr*)&ipv6->sin6_addr;
                ipver = "IPv6";
            }
            default: 
                break;
        }

        char ipstr[INET6_ADDRSTRLEN];

        if(!inet_ntop(temp->ai_family, addr, ipstr, sizeof(ipstr)))
            perror("inet_ntop() failed");

        if(connect(sockfd, (const struct sockaddr*)temp, sizeof(*temp)) == -1)
            printf("%s: connect() failed: %s\n", ipver, strerror(errno));
        else {
            printf("Connection established with: %s(%s)\n", ipstr, ipver);
            break;
        }
    }

    freeaddrinfo(addr_list);
    close(sockfd);
    exit(EXIT_SUCCESS);
}