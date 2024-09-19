#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "lib/error_handling.h"

#define BUF_SIZE 1024

int main(int argc, char **argv){
    int serv_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;
    char message[BUF_SIZE];

    if(argc != 2){
        error_handling("argc() error!");
    }

    if((serv_sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
        error_handling("socket() error!");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind() error!");
    }

    while(1){
        clnt_addr_len = sizeof(clnt_addr);
        int str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*) &clnt_addr, &clnt_addr_len);
        sendto(serv_sock, message, str_len, 0, (struct sockaddr*)& clnt_addr, clnt_addr_len);
    }
    close(serv_sock);
    return 0;
}