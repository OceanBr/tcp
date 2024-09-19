#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "lib/error_handling.h"

#define BUF_SIZE 1024

int main(int argc, char ** argv){
    if(argc != 2)
        error_handling("argc() error!");
    
    int sock;
    struct sockaddr_in my_addr, your_addr;
    socklen_t sockaddr_len;
    uint8_t str_len;
    char message[BUF_SIZE];

    if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
        error_handling("socket() error!");
    }
   
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(atoi(argv[1]));

    if(bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1){
        error_handling("bind() error!");
    }

    for(int i = 0; i < 3; i++){
        sleep(5);
        sockaddr_len = sizeof(your_addr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*) &your_addr, &sockaddr_len);
        printf("Message%d from host: %s\n", i + 1, message);
    }
    close(sock);
    return 0;
}
