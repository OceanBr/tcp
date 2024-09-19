#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include "lib/error_handling.h"

#define BUF_SIZE 1024

int main(int argc, char ** argv){
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    socklen_t serv_addr_len;

    if(argc != 3){
        error_handling("argc() error!");
    }

    if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
        error_handling("socket() error!");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    while ((1))
    {
        fputs("Insert message(q or Q quit) \n", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n")){
            break;
        }

        sendto(sock, message, strlen(message), 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
        serv_addr_len = sizeof(serv_addr);
        int str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*) &serv_addr, &serv_addr_len);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(sock);
    return 0;
    
}