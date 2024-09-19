#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 20

int main(){
    int filedes[2];
    pipe(filedes);
    pid_t pid = fork();
    char str[] = "hello world!";
    char buf[BUF_SIZE];

    if(pid == 0){
        write(filedes[1], str, sizeof(str));
    }else{
        read(filedes[0], buf, BUF_SIZE);
        printf("data from child process: %s\n", buf);
    }
}