#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "lib/error_handling.h"

#define BUF_SIZE 100

int main(int argc, char ** argv){
    if(argc != 3)
        error_handling("argc error!");
    
    int sock;
    struct sockaddr_in addr;
    FILE * fp;
    int read_cnt = 0;
    char buf[BUF_SIZE];

    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket() error!");
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        error_handling("connect() error!");
    
    fp = fopen("receive.txt", "wb");
    if(fp == NULL)
        error_handling("fopen() error!");

    while((read_cnt = read(sock, buf, BUF_SIZE)) != 0)
        fwrite((void*)buf, 1, read_cnt, fp);
    
    puts("Received file data");

    write(sock, "Thank you", 10);
    fclose(fp);
    close(sock);
    return 0;

}