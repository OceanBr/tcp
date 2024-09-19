#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define    BUF_SZIE 30

void error_handling(char * ss);

void read_childpro(int sig);

int main(int argc, char ** argv){
    if(argc != 2){
        printf("Usage : %s <port>", argv[0]);
        exit(1);
    }

    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t adr_sz;
    struct sigaction act;

    char message[BUF_SZIE];
    uint8_t strlen;

    act.sa_flags = 0;
    act.sa_handler = read_childpro;
    sigemptyset(&act.sa_mask);

    //注册信号
    int state = sigaction(SIGCHLD, &act, 0);
    if(state == -1){
        error_handling("sigaction() error!");
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1){
        error_handling("socket() error!");
    }

    //给服务器socket绑定地址
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
        error_handling("bind() error!");
    }

    if(listen(serv_sock , 5) == -1){
        error_handling("listen() error!");
    }
    
    while(1){
        adr_sz = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_sz);
        if(clnt_sock == -1){
            continue;
        }else{
            puts("new client connected...");
        }

        //多进程处理客户端连接
        pid_t pid = fork();
        if(pid == -1){
            close(clnt_sock);
            continue;
        }
        if(pid == 0){ //子进程
            close(serv_sock);
            while(strlen = read(clnt_sock, message, BUF_SZIE)){
                write(clnt_sock, message, strlen);
            }

            close(clnt_sock);
            puts("client disconnected...");
            return 0;

        }else{ //父进程
            close(clnt_sock);
        }

    }
    return 0;
}

void error_handling(char *ss){
    fputs(ss, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childpro(int sig){
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}