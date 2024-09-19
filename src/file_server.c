#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "lib/error_handling.h"

#define BUF_SIZE 100

int main(int argc, char ** argv){
    if(argc != 2)
        error_handling("argc error!");
    
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;
    FILE * fp;
    uint8_t read_cnt = 0;
    char buf[BUF_SIZE];

    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket() error!");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr) )== -1)
        error_handling("bind() error!");
    
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error!");
    
    fp = fopen("bound_host1.c", "rb");
    if(fp == NULL)
        error_handling("fopen() error!");

    clnt_addr_len = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*) & clnt_addr, &clnt_addr_len);
    if(clnt_sock == -1)
        error_handling("accept() error!");
    
    while(1){
        read_cnt = fread((void*)buf, 1, BUF_SIZE, fp);
        if(read_cnt <BUF_SIZE){
            write(clnt_sock, buf, read_cnt);
            break;
        }
        write(clnt_sock, buf, read_cnt);
    }
    shutdown(clnt_sock, SHUT_WR);
    int read_len = read(clnt_sock, buf, BUF_SIZE);
    buf[read_len] = 0;
    printf("Message from client: %s \n", buf);
    close(clnt_sock);
    close(serv_sock);
    return 0;
}