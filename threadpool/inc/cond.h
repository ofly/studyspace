#ifndef THREADPOOL_COND_T
#include <pthread.h>
#include <sys/time.h>

typedef struct cond {
    pthread_mutex_t p_lock; // mutex
    pthread_cond_t  p_cond; // condition variable
} cond_t;

int cond_init(cond_t *cond);        // initial cond
int cond_destroy(cond_t *cond);     // destroy cond
int cond_lock(cond_t *cond);        // acquire lock
int cond_unlock(cond_t *cond);      // release lock
int cond_wait(cond_t *cond);        // wait for a condition
int cond_timewait(cond_t *cond, const struct timespec *ts);   // wait for a condition for a specified time
int cond_signal(cond_t *cond);      // send signal to a waiting thread
int cond_broadcast(cond_t *cond);   // broadcast singal to waiting threads

#define THREADPOOL_COND_T
#endif
