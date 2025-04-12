#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

int dangerous_flag = 0;
int finished = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Hàm lấy thời gian hiện tại
void print_current_time() {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%H:%M:%S", tm_info);
    printf("[%s] ", buffer);
}

// Kiểm tra trạng thái an toàn
int is_safe() {
    return (dangerous_flag == 0);
}

// Hành động khôi phục
void perform_recovery() {
    print_current_time();
    printf("[WARNING]: System unsafe! Performing recovery...\n");
    dangerous_flag = 0; // reset trạng thái
}

// Hàm kiểm tra định kỳ
void* periodical_detector(void *arg) {
    int cnt = 0;
    while (cnt != 4) {
        sleep(5);
        pthread_mutex_lock(&lock);
        if (!is_safe()) {
            perform_recovery();
            if (!finished) {
                pthread_mutex_unlock(&lock);
            }
            if (finished && is_safe()) {
                pthread_mutex_unlock(&lock);
                break;
            }
        }
        pthread_mutex_unlock(&lock);
        cnt++;
    if (cnt == 21) return NULL;
    }

    return NULL;
}

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, periodical_detector, NULL);

    // Vòng lặp mô phỏng hoạt động chính
    for (int i = 0; i < 20; ++i) {
        pthread_mutex_lock(&lock);

        print_current_time();
        printf("Tick %02d: System running...\n", i);

        if (i == 7 || i == 13) {
            printf("[DANGER] Dangerous event triggered at i = %d\n", i);
            dangerous_flag = 1;
        }
        printf("Dangerous flag: %d \n", dangerous_flag);

        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    // Thông báo kết thúc
    pthread_mutex_lock(&lock);
    finished = 1;
    pthread_mutex_unlock(&lock);

    pthread_join(tid, NULL);
    pthread_mutex_destroy(&lock);

    printf("\n[DONE] System finished safely. All resources released.\n");
    return 0;
}
