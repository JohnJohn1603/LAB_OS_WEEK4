#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h> 

#define NUM_THREADS 4
#define NUM_OPS_PER_THREAD 5

// Cac Node trong Stack
typedef struct Node {
    int value;
    struct Node* next;
} Node;

// Stack su dung Atomic Pointer
typedef struct LockFreeStack {
    _Atomic(Node*) head;
} LockFreeStack;

// Khoi tao stack
void init_stack(LockFreeStack* stack) {
    atomic_store(&stack->head, NULL);
}

// Push lock-free
bool push(LockFreeStack* stack, int value) {
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) return false;
    new_node->value = value;

    Node* old_head;
    do {
        old_head = atomic_load(&stack->head);
        new_node->next = old_head;
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, new_node));
    return true;
}

// Pop lock-free
bool pop(LockFreeStack* stack, int* value) {
    Node* old_head;
    Node* next;
    do {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) return false;
        next = old_head->next;
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, next));
    *value = old_head->value;
    free(old_head);
    return true;
}

// Thread function
void* thread_func(void* arg) {
    LockFreeStack* stack = (LockFreeStack*)arg;

    for (int i = 0; i < NUM_OPS_PER_THREAD; ++i) {
        int val = i + 1 + rand() % 100;
        push(stack, val);
        printf("[PUSH] [Thread %lu] Pushed: %d\n", pthread_self(), val);
        usleep(1000000); // sleep 1s

        int popped;
        if (pop(stack, &popped)) {
            printf("[POP] [Thread %lu] Popped: %d\n", pthread_self(), popped);
        } else {
            printf("[EMPTY] [Thread %lu] Stack empty\n", pthread_self());
        }
        usleep(1000000);
    }

    return NULL;
}

int main() {
    LockFreeStack stack;
    init_stack(&stack);

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_func, &stack);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("[DONE*] All threads done.\n");

    // in phan con lai (neu co)
    int val;
    while (pop(&stack, &val)) {
        printf("[REMAINING] Remaining: %d\n", val);
    }

    return 0;
}
