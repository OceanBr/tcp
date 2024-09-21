#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void *request_handler(void *arg);
void send_data(FILE *fp, char *ct, char *file_name);
char *content_type(char *file);
void send_error(FILE *fp);
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_size;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    int option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (const void*)&option, sizeof(option));
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 20) == -1)
        error_handling("listen() error");

    while (1)
    {
        clnt_adr_size = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_size);
        if (clnt_sock == -1)
            continue;

        printf("Connection Request : %s:%d\n",
               inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));

        int *sock_ptr = malloc(sizeof(int)); // 为每个线程分配独立的客户端 socket 变量
        *sock_ptr = clnt_sock;

        pthread_create(&t_id, NULL, request_handler, (void *)sock_ptr);
        pthread_detach(t_id); // 不需要等待线程结束
    }

    close(serv_sock);
    return 0;
}

void *request_handler(void *arg)
{
    int clnt_sock = *((int *)arg);
    free(arg); // 释放动态分配的内存

    char req_line[SMALL_BUF];
    FILE *clnt_read;
    FILE *clnt_write;

    char method[10];
    char ct[15];
    char file_name[30];

    clnt_read = fdopen(clnt_sock, "r");
    clnt_write = fdopen(dup(clnt_sock), "w");

    if (fgets(req_line, SMALL_BUF, clnt_read) == NULL)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    if (strstr(req_line, "HTTP/") == NULL)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    strcpy(method, strtok(req_line, " /"));
    strcpy(file_name, strtok(NULL, " /"));

    if (file_name == NULL || method == NULL)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    strcpy(ct, content_type(file_name));

    if (strcmp(method, "GET") != 0)
    {
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    fclose(clnt_read);
    send_data(clnt_write, ct, file_name);
    return NULL;
}

void send_data(FILE *fp, char *ct, char *file_name)
{
    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];
    FILE *send_file;

    sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
    
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    // printf("dir: %s\n", cwd);
    // int str_len = strlen(cwd);
    // cwd[str_len] = '/';
    // cwd[str_len+ 1] = '\0';
    // printf("dir: %s\n", cwd);
    // strcat(cwd,file_name);
    // printf("dir: %s\n", cwd);

    
    // 打印请求的文件名，便于调试
    
    //strcpy(cwd , "/home/iotocean/code/tcpip/index.html");
    printf("Requested dir: %s\n", cwd);
    printf("Requested file: %s\n", file_name);
    send_file = fopen(file_name, "r");
    if (send_file == NULL)
    {
        send_error(fp);
        return;
    }

    // 传输头信息
    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_type, fp);

    // 传输请求数据
    while (fgets(buf, BUF_SIZE, send_file) != NULL)
    {
        fputs(buf, fp);
        fflush(fp);
    }

    fclose(send_file);
    fflush(fp);
    fclose(fp);
}

char *content_type(char *file)
{
    char *extension = strrchr(file, '.'); // 使用 strrchr 获取文件扩展名
    if (extension == NULL)
        return "text/plain";

    if (!strcmp(extension, ".html") || !strcmp(extension, ".htm"))
        return "text/html";
    else
        return "text/plain";
}

void send_error(FILE *fp)
{
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";
    char content[] = "<html lang = \"zh\"><head><meta charset=\"UTF-8\"><title>NETWORK ERROR</title></head>"
                     "<body><font size=+5><br>发生错误！ 查看请求文件名和请求方式!"
                     "</font></body></html>";

    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_type, fp);
    fputs(content, fp);
    fflush(fp);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
