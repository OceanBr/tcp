#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char ** argv){
    fd_set reads, temp;
    
    FD_ZERO(&reads);
    FD_SET(0, &reads);

    struct timeval timeout;
    char buf[BUF_SIZE];

    int result;

    while(1){
        temp = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select(1, &temp, 0, 0, &timeout);
        //printf("%d\n", result);
        if(result == -1){
            puts("select() error!");
            exit(1);
        }else if( result == 0){
            puts("time out!");
        }else{
            if(FD_ISSET(0, &temp)){
                int str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console : %s\n", buf);
            }
        }
    }
    return 0;

}