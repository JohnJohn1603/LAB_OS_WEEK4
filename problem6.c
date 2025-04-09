#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_RESOURCES 7      
#define NUM_PROCESSES 3      

typedef struct {
    int id;                                 
    int requested_resources;           
    void (*callback)(int);                  
} process_request_t;

int available_resources = NUM_RESOURCES;
pthread_mutex_t resource_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resource_cond = PTHREAD_COND_INITIALIZER;

// call back: in ra khi da allocate xong
void resource_callback(int process_id) {
    printf("[CALLBACK] Process %d has been allocated resources.\n", process_id);
    // gia su ton 2 giay
    sleep(2);
}

void* resource_man(void* arg) {
    process_request_t* request = (process_request_t*) arg;

    pthread_mutex_lock(&resource_lock);
    while (request->requested_resources > available_resources) {
        printf("[WAITING] Process %d waiting...\n", request->id);
        pthread_cond_wait(&resource_cond, &resource_lock);
    }

    // Allocate
    available_resources -= request->requested_resources;
    printf("[NOTIFICATION] Process %d granted %d resources (Remaining: %d)\n",
            request->id, request->requested_resources, available_resources);

    // Lock som -> cho phep cac thread khac truy cap vao
    pthread_mutex_unlock(&resource_lock);

    // Co che callback 
    request->callback(request->id);

    // Tra lai tai nguyeyen dung chung
    pthread_mutex_lock(&resource_lock);
    available_resources += request->requested_resources;
    printf("[NOTIFICATION] Process %d released %d resources (Now Available: %d)\n",
            request->id, request->requested_resources, available_resources);
    pthread_cond_broadcast(&resource_cond);
    pthread_mutex_unlock(&resource_lock);

    return NULL;
}

int main() {
    pthread_t threads[NUM_PROCESSES];

    process_request_t requests[NUM_PROCESSES] = {
        { .id = 1, .requested_resources = 3, .callback = resource_callback },
        { .id = 2, .requested_resources = 5, .callback = resource_callback },
        { .id = 3, .requested_resources = 1, .callback = resource_callback },
    };

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pthread_create(&threads[i], NULL, resource_man, (void*)&requests[i]);
        sleep(1); 
    }

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("🏁 All processes finished.\n");
    return 0;
}
