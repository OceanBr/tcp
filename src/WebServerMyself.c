#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <assert.h>

const size_t BUF_SIZE = 1024;
const size_t REQ_SIZE = 100;

void* request_handler(void* arg);
void send_error(FILE* fp);
void send_data(FILE* clnt_write, char* filename, char* ct);
char* content_type(char* file_name);

int main(int argc, char* argv[]){
    assert(argc == 2);

    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_size;
    pthread_t tid;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    assert(serv_sock != -1);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    assert((bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))) != -1);

    assert((listen(serv_sock, 10)) != -1);

    while(1){
       clnt_adr_size = sizeof(clnt_adr);
       //accept是阻塞的吗？
       clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
       if(clnt_sock == -1){
        continue;
       }
       printf("Connection Request : %s:%d\n",
               inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));

       int *sock_ptr = malloc(sizeof(int)); // 为每个线程分配独立的客户端 socket 变量
        *sock_ptr = clnt_sock;
       pthread_create(&tid, NULL, request_handler, (void*)sock_ptr);
       pthread_detach(tid); 
    }
    close(serv_sock);
    return 0;
}

void* request_handler(void* arg){
    int clnt_sock = *((int *)arg);
    free(arg);
    FILE* clnt_read = fdopen(clnt_sock, "r");
    FILE* clnt_write = fdopen(dup(clnt_sock), "w");

    if (clnt_read == NULL || clnt_write == NULL) {
        perror("fdopen failed");
        close(clnt_sock);
        return NULL;
    }


    char req_line[REQ_SIZE];//存储请求行
    fgets(req_line, REQ_SIZE, clnt_read);
    if(strstr(req_line, "HTTP/") == NULL){
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }
    
    //fclose(clnt_read);

    char method[10];
    char ct[15];
    char file_name[20];
    
    strcpy(method, strtok(req_line, " /"));
    //printf("method : %s\n", method);
    
    strcpy(file_name, strtok(NULL, " /"));
    //printf("file name : %s\n", file_name);
    
    strcpy(ct, content_type(file_name));
    //printf("content type : %s\n", ct);
    
    if (file_name == NULL || method == NULL)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    if(strcmp(method, "GET") != 0){
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    fclose(clnt_read);
    send_data(clnt_write, file_name, ct);
    return NULL;
}

void send_data(FILE* clnt_write, char* file_name, char* ct){
    char buf[BUF_SIZE];
    char protocol[] = "HTTP/1.1 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    //char* cnt_len = "Content-length:2048\r\n";
    char content_type[100];
    sprintf(content_type, "Content-type : %s\r\n\r\n", ct);

    FILE* send_file = fopen(file_name, "r");
    if(send_file == NULL){
        send_error(clnt_write);
        return;
    }

    //传输头部信息
    fputs(protocol, clnt_write);
    fputs(server, clnt_write);
    //fputs(cnt_len, clnt_write);
    fputs(content_type, clnt_write);

    while(fgets(buf, BUF_SIZE, send_file) != NULL){
        fputs(buf, clnt_write);
        fflush(clnt_write);
    }
    fclose(send_file);
    fflush(clnt_write);
    fclose(clnt_write);
}

void send_error(FILE* clnt_write){
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char content_type[] = "Content-type:text/html\r\n\r\n";
    fputs(protocol, clnt_write);
    fputs(server, clnt_write);
    fputs(content_type, clnt_write);
    fflush(clnt_write);

    char buf[BUF_SIZE];
    FILE* fp = fopen("error.html", "r");
    while(fgets(buf, BUF_SIZE, fp) != NULL){
        fputs(buf, clnt_write);
        fflush(clnt_write);
    }
    fclose(fp);
    fflush(clnt_write);
    fclose(clnt_write);
    return ;
}

char* content_type(char* file_name){
    char* extension = strrchr(file_name, '.');
    puts(extension);
    if(!strcmp(extension, ".html") || !strcmp(extension, ".htm")){
        return "text/html";
    }else{
        return "text/plain";
    }
}