#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void write(int value){
    pthread_mutex_lock(&mutex);
    while(count == BUFFER_SIZE) pthread_cond_wait(&not_full, &mutex);
    buffer[count++] = value;
    printf("Write: %d\n", value);

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}

int read(){
    pthread_mutex_lock(&mutex);
    while(count == 0) pthread_cond_wait(&not_empty, &mutex);
    int value = buffer[--count];
    printf("Read: %d\n", value);
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
    return value;
}

void* producer(void* arg) {
    for (int i = 1; i <= 20; i++) {
        write(i);
        usleep(100000);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 1; i <= 20; i++) {
        read();
        usleep(150000);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    printf("All done!\n");
    return 0;
}
