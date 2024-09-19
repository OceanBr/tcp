#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "lib/error_handling.h"

#define BUF_SIZE 1024
#define LOG_SIZE 5
#define OP_SIZE 4

int calculate(int opnum, int * opnds, char op);

int main(int argc, char* argv[]){
    if(argc != 2)
        error_handling("argc error!");
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char message[BUF_SIZE];
    int recv_len;
    char opinfo[BUF_SIZE];

    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket error!");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if((bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr))) == -1)
        error_handling("bind error!");
    
    if((listen(serv_sock, LOG_SIZE)) == -1)
        error_handling("listen error!");
    
    clnt_addr_size = sizeof(clnt_addr);
    
    for(int i = 0; i < 5; i++){
        int opnd_cnt = 0;
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1)
            error_handling("accept() error!");
        else
            printf("Client%d connected----------------\n", i);
        read(clnt_sock, &opnd_cnt, 1);
        recv_len = 0;
        while(opnd_cnt * OP_SIZE + 1 > recv_len){
            int recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE);
            recv_len += recv_cnt;
        }

        int result = calculate(opnd_cnt, (int*) opinfo, opinfo[recv_len - 1]);
        printf("%d\n", result);
        write(clnt_sock, &result, sizeof(result));
        close(clnt_sock);
    }

    
    close(serv_sock);

}

int calculate(int opnum, int* opnds, char op){
    int result = opnds[0];
    switch (op){
        case '+' :
            for(int i = 1; i < opnum; i++){
                result += opnds[i];
            }       
            break;
        case '-' :
            for(int i = 1; i < opnum; i++)
                result -= opnds[i];
            break;
        case '*' :
            for(int i = 1; i < opnum; i++)
                result *= opnds[i];
            break;
    }
    return result;
}
