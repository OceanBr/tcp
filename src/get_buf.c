#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *message){
    fputs(message, stdout);
    fputc('\n', stderr);
    exit(1);
}

int main(){
    int sock;
    int snd_buf, recv_buf, state;
    socklen_t len;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    len = sizeof(snd_buf);
    state = getsockopt(sock, SOL_SOCKET,SO_SNDBUF, (void *)&snd_buf, &len);
    if(state)
        error_handling("getsockopt() error!");
    
    len = sizeof(recv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recv_buf, &len);
    if(state)
        error_handling("getsockopt() error");

    printf("Input buffer size: %d \n", recv_buf);
    printf("Output buffer size: %d \n", snd_buf);
    return 0; 


}