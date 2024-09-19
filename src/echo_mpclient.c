#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 30

void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);
void error_handling(char * ss);

int main(int argc, char ** argv){
    int sock;
    struct sockaddr_in sock_addr;
    char buf[BUF_SIZE];

    if(argc != 3){
        error_handling("argc error");
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sock_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1){
        error_handling("connect() error!");
    }

    pid_t pid = fork();
    if(pid == 0){
        //子程序
        write_routine(sock, buf);
    }else{
        //父程序
        read_routine(sock, buf);
    }
    close(sock);
    return 0;
}

void write_routine(int sock, char *buf){
    while(1){
        puts("Input message:");
        fgets(buf, BUF_SIZE, stdin);
        if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){
            shutdown(sock, SHUT_WR);
            return ;
        }
        write(sock, buf, strlen(buf));
    }
}
void read_routine(int sock, char* buf){
    while(1){
        int str_len = read(sock, buf, BUF_SIZE);
        if(str_len == 0){
            return ;
        }   
        buf[str_len] = 0;
        printf("Message from server: %s\n", buf);
    }
}

void error_handling(char * ss){
    printf("%s\n", ss);
    exit(1);
}