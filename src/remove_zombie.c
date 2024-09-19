#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if(WIFEXITED(status)){
        printf("Removed proc id: %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}

int main(){
    pid_t pid;
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);

    sigaction(SIGCHLD, &act, 0);

    pid = fork();
    
    if(pid == 0){
        puts("hi, i am child process");
        sleep(5);
        return 12;
    }else{
        printf("child process id: %d \n", pid);
        pid = fork();
        if(pid == 0){
            puts("hi, i am child proces too");
            sleep(10);
            exit(24);
        }else{
            printf("child process id: %d \n", pid);
            for(int i = 0; i < 5; i++){
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}