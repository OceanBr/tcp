#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int sum;
void * thread_main(void * arg);

int main(){
    int range1[] = {1, 5};
    int range2[] = {6, 10};
    pthread_t t_id1, t_id2;

    pthread_create(&t_id1, NULL, thread_main, (void *)range1);
    pthread_create(&t_id2, NULL, thread_main, (void *) range2);

    pthread_join(t_id1, NULL);
    pthread_join(t_id2, NULL);

    printf("result: %d \n", sum);
    return 0;
}

void * thread_main(void * arg){
    int start = ((int *)arg)[0];
    int end = ((int *)arg)[1];

    for(; start <= end; start++){
        sum += start;
    }
    return NULL;
}