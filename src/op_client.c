#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "lib/error_handling.h"

#define BUF_SIZE   1024
#define OP_SIZE 4

int main(int argc, char ** argv){
    if(argc != 3)
        error_handling("argc error!");
    
    int sock;
    struct sockaddr_in sock_addr;

    int opnd_cnt = 0;
    int result = 0;
    char opmsg[BUF_SIZE];

    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket() error!");
    
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(argv[1]);
    sock_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*) &sock_addr, sizeof(sock_addr)) == -1)
        error_handling("connect() error!");
    else
        printf("connected---------------\n");
    
    fputs("Operand count: ", stdin);
    scanf("%d", &opnd_cnt);
    opmsg[0] = (char)opnd_cnt;

    for(int i = 0; i < opnd_cnt; i++){
        printf("Operand %d:", i + 1);
        scanf("%d", (int *)&opmsg[i * OP_SIZE + 1]);
    }
    fgetc(stdin);//回车
    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[opnd_cnt * OP_SIZE + 1]);
    write(sock, opmsg, OP_SIZE * opnd_cnt + 2);
    read(sock, &result, 4);
    printf("Operation result: %d\n", result);

    close(sock);
}