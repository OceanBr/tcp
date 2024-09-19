#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "lib/error_handling.h"

#define BUF_SIZE 1024

int main(int argc, char ** argv){
    if(argc != 3){
        error_handling("argc() error!");
    }

    char msg1[] = "hello";
    char msg2[] = "world";
    char msg3[] = "!!!";

    int sock;
    struct sockaddr_in your_addr;
    socklen_t your_addr_len;
    
    if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
        error_handling("socket() error!");
    }

    memset(&your_addr, 0, sizeof(your_addr));
    your_addr.sin_family = AF_INET;
    your_addr.sin_addr.s_addr = inet_addr(argv[1]);
    your_addr.sin_port = htons(atoi(argv[2]));

    sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr*)&your_addr, sizeof(your_addr));
    sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*)&your_addr, sizeof(your_addr));
    sendto(sock, msg3, sizeof(msg3), 0, (struct sockaddr*)&your_addr, sizeof(your_addr));
    
    close(sock);
    return 0;
}