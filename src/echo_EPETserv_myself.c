#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#define BUF_SIZE 4
#define EPOLL_SIZE 50

void setnonblockingmode(int fd);

int main(int argc, char ** argv){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz;

    int str_len;
    char buf[BUF_SIZE];

    int epfd;
    struct epoll_event * ep_events;
    struct epoll_event event;
    int event_cnt;

    if(argc != 2){
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr));
    listen(serv_sock, 5);

    epfd = epoll_create1(EPOLL_CLOEXEC);
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    setnonblockingmode(serv_sock);
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while(1){
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1){
            puts("epoll_wait() error");
            break;
        }

        for(int i = 0; i < event_cnt; i++){
            if(ep_events[i].data.fd == serv_sock){
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                event.events = EPOLLIN|EPOLLET;
                event.data.fd = clnt_sock;
                setnonblockingmode(clnt_sock);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("new client connected : %d \n", clnt_sock);
            }else{
                while(1){
                    int temp_sock = ep_events[i].data.fd;
                    str_len = read(temp_sock, buf, BUF_SIZE);
                    if(str_len < 0){
                        if(errno == EAGAIN){
                            break;
                        }
                    }else if(str_len == 0){  
                        epoll_ctl(epfd, EPOLL_CTL_DEL, temp_sock, NULL);
                        close(temp_sock);
                        printf("closed client : %d \n", temp_sock);
                        break;
                    }else{
                        write(temp_sock, buf, str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void setnonblockingmode(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}