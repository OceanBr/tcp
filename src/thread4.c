#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define THREAD_NUM 100

long long num;
void* thread_inc(void *arg);
void* thread_des(void *arg);

int main(){
    pthread_t tid[THREAD_NUM];
    for(int i = 0; i < THREAD_NUM; i++){
        if(i % 2)
            pthread_create(&tid[i], NULL, thread_inc, NULL);
        else
            pthread_create(&tid[i], NULL, thread_des, NULL);
    }

    for(int i = 0; i < THREAD_NUM; i++){
        pthread_join(tid[i], NULL);
    }
    printf("num: %lld \n", num);
    return 0;
}

void* thread_inc(void * arg){
    for(int i = 0; i < 50000000; i++)
        num += 1;
    return NULL;
}

void * thread_des(void * arg){
    for(int i = 0; i < 50000000; i++)
        num -= 1;
    return NULL;
}