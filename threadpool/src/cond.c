#include <stdio.h>
#include <stdlib.h>
#include "cond.h"

int cond_init(cond_t* cond) {
    int ret;
    ret = pthread_mutex_init(&cond->p_lock, NULL);
    if (ret) {
        fprintf(stderr, "pthread_mutex_init: %d\n", ret);
        exit(ret);
    }
    ret = pthread_cond_init(&cond->p_cond, NULL);
    if (ret) {
        fprintf(stderr, "pthread_cond_init: %d\n", ret);
        exit(ret);
    }
    return 0;
}

int cond_destroy(cond_t* cond) {
    int ret;
    ret = pthread_mutex_destroy(&cond->p_lock);
    if (ret) {
        fprintf(stderr, "pthread_mutex_destroy: %d\n", ret);
        exit(ret);
    }
    ret = pthread_cond_destroy(&cond->p_cond);
    if (ret) {
        fprintf(stderr, "pthread_cond_destroy: %d\n", ret);
        exit(ret);
    }
    return 0;
}

int cond_lock(cond_t* cond) {
    return pthread_mutex_lock(&cond->p_lock);
}

int cond_unlock(cond_t* cond) {
    return pthread_mutex_unlock(&cond->p_lock);
}

int cond_wait(cond_t* cond) {
    return pthread_cond_wait(&cond->p_cond, &cond->p_lock);
}

int cond_timewait(cond_t* cond, const struct timespec *ts) {
    return pthread_cond_timedwait(&cond->p_cond, &cond->p_lock, ts);
}

int cond_signal(cond_t* cond) {
    return pthread_cond_signal(&cond->p_cond);
}

int cond_broadcast(cond_t* cond) {
    return pthread_cond_broadcast(&cond->p_cond);
}
