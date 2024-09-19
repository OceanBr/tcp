#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void * read(void * arg);
void * accu(void * arg);
static sem_t sem1, sem2;
static int num;

int main(){
    pthread_t tid1, tid2;
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 1);

    pthread_create(&tid1, NULL, read, NULL);
    pthread_create(&tid2, NULL, accu, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&sem1);
    sem_destroy(&sem2);
    return 0;
}

void * read(void * arg){
    for(int i = 0; i < 5; i++){
        fputs("Input num:", stdout);
        sem_wait(&sem2);
        scanf("%d", &num);
        sem_post(&sem1);
    }
    return NULL;
}

void * accu(void * arg){
    int sum = 0; 
    for(int i = 0; i < 5; i++){
        sem_wait(&sem1);
        sum += num;
        sem_post(&sem2);
    }
    printf("Result : %d \n", sum);
    return NULL;
}