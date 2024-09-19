#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE  1024

void error_handling(char* ss);

int main(int argc, char ** argv){
    if(argc != 3)
        error_handling("argc error!");

    int sock;
    struct sockaddr_in socket_addr;

    char message[BUF_SIZE];

    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket error!");
    
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = inet_addr(argv[1]);
    socket_addr.sin_port = htons(atoi(argv[2]));

    if((connect(sock, (struct sockaddr*) &socket_addr, sizeof(socket_addr))) == -1)
        error_handling("connect error!");
    else
        puts("connected----");
    
    while(1){
        fputs("Input message(q or Q quit) \n", stdout);
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        int str_len = write(sock, message, strlen(message) + 1);
        int recv_len = 0;
        while(recv_len < str_len){
            int recv_cnt = read(sock, message, BUF_SIZE);
            if(recv_cnt == -1)
                error_handling("read error!");
            recv_len += recv_cnt;
        }
        printf("Message from server:\n%s", message);
    }
    close(sock);
    return 0;
}

void error_handling(char* ss){
    printf("%s\n", ss);
    exit(1);
}