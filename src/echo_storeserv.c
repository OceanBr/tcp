#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#define BUF_SIZE 100

void error_handling(char * ss);
void read_childprocess(int sig);

int main(int argc, char ** argv){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t sock_len;
    int str_len;

    pid_t pid;
    int fds[2];

    char buf[BUF_SIZE]; 

    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = read_childprocess;
    sigaction(SIGCHLD, &act, 0);

    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr));

    listen(serv_sock, 5);

    pipe(fds);

    pid = fork();
    if(pid == 0){
        FILE* fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int len;
        for(int i= 0; i < 10; i++){
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void *)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }
    while(1){
        sock_len = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &sock_len);
        if(clnt_sock == -1){
            continue;
        }else{
            puts("new client connected!");
        }
        pid = fork();
        if(pid == 0){
            close(serv_sock);
           
            while( (str_len = read(clnt_sock, buf, BUF_SIZE)) != 0){
                buf[str_len] = 0;
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);
            }
            close(clnt_sock);
            puts("client disconnected....");
            return 0;
        }else{
            close(clnt_sock);
        }
        
    }
    close(serv_sock);
    return 0;
}

void error_handling(char * ss){
    printf("%s\n", ss);
    exit(1);
}

void read_childprocess(int sig){
    int status;
    pid_t pid = waitpid(-1, &status, 0);
    printf("the disconnected client process id: %d\n", pid);
    return;
}
