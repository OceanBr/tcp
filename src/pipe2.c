#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(){
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    __pid_t pid;

    pipe(fds);
    pid = fork();

    if(pid == 0){
        write(fds[1], str1, sizeof(str1));
        //sleep(2);
        int len = read(fds[0], buf , BUF_SIZE);
        buf[len] = 0;
        printf("Child process output: %s \n", buf);
        
    }else{
        int len = read(fds[0], buf, BUF_SIZE);
        buf[len] = 0;
        printf("Parent process output: %s \n", buf);
        write(fds[1], str2, sizeof(str2));
        //sleep(3);
    }
    return 0;

}