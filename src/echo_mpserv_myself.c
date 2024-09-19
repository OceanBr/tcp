#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUF_SIZE 30

void error_handling(char * ss);
void read_childprocess(int sig);

int main(int argc, char ** argv) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t len;
    struct sigaction act;
    uint8_t strlen;
    char buf[BUF_SIZE];
    
    if(argc != 2){
        printf("Usage: %s<port>", argv[1]);
        exit(1);
    }

    //注册信号
    act.sa_flags = 0;
    act.sa_handler = read_childprocess;
    sigemptyset(&act.sa_mask);
    int state = sigaction(SIGCHLD, &act, 0);

    //生成服务器socket
    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        error_handling("socket() error!");
    }
    
    //服务器地址绑定socket
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*) & serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind() error!");
    }
    
    //服务器socket监听端口，设置缓冲大小
    if(listen(serv_sock, 5) == -1){
        error_handling("listen() error!");
    }

    len = sizeof(clnt_addr);
    while(1){
        //与客户端建立连接，生成客户端socket
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &len);
        if(clnt_sock == -1){
            continue;
        }else{
            puts("new client connected...");
        }
        pid_t pid = fork();
        if(pid == -1){
            continue;
        }else if(pid == 0){ //子程序
            close(serv_sock);
            while(strlen = read(clnt_sock, &buf, BUF_SIZE)){
                buf[strlen] = 0;
                printf("data from client: %s \n", buf);
                write(clnt_sock, buf, strlen);
            }
            close(clnt_sock);
            return 0;
        }else{//父程序
            close(clnt_sock);
        }

    }

    close(serv_sock);
    return 0;
}

void error_handling(char *ss){
    fputs(ss, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childprocess(int sig){
    int status;
    pid_t pid = waitpid(-1, &status, 0);
    printf("the disconnected client process id : %d \n", pid);
    return;
}