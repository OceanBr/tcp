#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(){
    int fds1[2], fds2[2];
    char buf[BUF_SIZE];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";

    pipe(fds1);
    pipe(fds2);

    __pid_t pid = fork();
    if(pid == 0){
        write(fds1[1], str1, sizeof(str1));
        int len = read(fds2[0], buf, BUF_SIZE);
        buf[len] = '\0';
        printf("Child process output: %s\n", buf);
    }else{
        int len = read(fds1[0], buf, BUF_SIZE);
        buf[len] = 0;
        printf("Parent process output: %s\n", buf);
        write(fds2[1], str2, sizeof(str2));
        sleep(3);
    }
    return 0;
}