#ifndef PTHREAD_H
#include <pthread.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef struct pthread_seqlock {
    atomic_int seq;  // Sequence number
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
    atomic_init(&rw->seq, 0);  // Khởi tạo seq với giá trị 0
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
    atomic_fetch_add(&rw->seq, 1);  // Bắt đầu ghi
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
    atomic_fetch_add(&rw->seq, 1);  // Kết thúc ghi
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
    int seq1, seq2;

    do {
        seq1 = atomic_load(&rw->seq);  // Đọc giá trị seq đầu tiên
        while (seq1 & 1) {  // Nếu seq là số lẻ (writer đang ghi), tiếp tục đọc lại
            seq1 = atomic_load(&rw->seq);
        }

        seq2 = atomic_load(&rw->seq);  // Đọc lại seq sau khi đọc dữ liệu
    } while (seq1 != seq2);  // Nếu seq thay đổi, quay lại

    return 1;
}

static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
    return 0;  // Không cần làm gì vì reader không thay đổi seq
}
