#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define THREAD_NUM 100

void * thread_inc(void * arg);
void * thread_des(void * arg);

long long num;
pthread_mutex_t mutex;

int main(){
    pthread_t tid[THREAD_NUM];
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < THREAD_NUM; i++){
        if(i % 2)
            pthread_create(&tid[i], NULL, thread_inc, NULL);
        else   
            pthread_create(&tid[i], NULL, thread_des, NULL);
    }

    for(int i = 0; i < THREAD_NUM; i++){
        pthread_join(tid[i], NULL);
    }

    printf("result: %lld\n", num);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void * thread_inc (void * arg){
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < 50000000; i++){
        num += 1;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void * thread_des(void * arg){
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < 50000000; i++){
        num -= 1;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}