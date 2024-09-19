#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
#define LOG_NUM 5

void error_handling(char* ss){
    printf("%s\n", ss);
    exit(1);
}

int main(int argc, char** agrv){
    if(argc != 2)
        error_handling("argc error!");

    char message[BUF_SIZE];
    
    int server_socket, client_socket;
    int str_len = 0;
    struct sockaddr_in  server_addr, client_addr;

    server_socket = socket(PF_INET, SOCK_STREAM, 0); 
    if(server_socket == -1)
        error_handling("socket erro!");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(agrv[1]));

    if(bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
        error_handling("bind error!");
    
    if(listen(server_socket, LOG_NUM) == -1)
        error_handling("listen error!");
    
    int client_addr_count = sizeof(client_addr);

    for(int i = 0; i < LOG_NUM; i++){    
        client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &client_addr_count);
        if(client_socket == -1)
            error_handling("accept error!");
        else
            printf("Connected client %d \n", i+ 1);
        while((str_len = read(client_socket, message, BUF_SIZE)) != 0){
            printf("Message from client%d: %s", i + 1, message);
            printf("the char number of the message:%ld\n", strlen(message) - 1);
            write(client_socket, message, str_len);
        }
        close(client_socket);
    }
    close(server_socket);
    return 0;
}