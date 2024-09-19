#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *ss);

int main(int argc, char ** argv){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t sock_len;
    char buf[BUF_SIZE];

    fd_set reads, reads_temp;
    struct timeval timeout;
    int state;

    if(argc != 2){
        printf("Usage %s <port>!\n", argv[0]);
        exit(1);
    }

    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        error_handling("socket() error!");
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("bind() error!");
    }

    if(listen(serv_sock, 5) == -1){
        error_handling("listen() error!");
    }

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    int fd_max = serv_sock;

    while(1){
        reads_temp = reads;
        timeout.tv_sec = 8;
        timeout.tv_usec = 100;

        state = select(fd_max + 1, &reads_temp, 0, 0, &timeout);

        if(state == -1){
            break;
        }else if(state == 0){
            continue;
        }else{ //读有变化
            for(int i = 0;  i < fd_max + 1; i++){
                if(FD_ISSET(i, &reads_temp)){
                    if(i == serv_sock){ // 新的连接
                        sock_len = sizeof(clnt_adr);
                        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &sock_len);
                        FD_SET(clnt_sock, &reads);
                        if(clnt_sock > fd_max){
                            fd_max = clnt_sock;
                       }
                        printf("new connected client : %d \n", clnt_sock);
                    }else{ // 读数据
                        int str_len = read(i, buf, BUF_SIZE);
                        if(str_len == 0){ // 读结束了
                            FD_CLR(i, &reads);
                            close(i);
                            printf("closed client : %d \n", i);
                       }else{
                            write(i, buf, str_len);
                        }
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;

}

void error_handling(char *ss){
    puts(ss);
    exit(1);
}
