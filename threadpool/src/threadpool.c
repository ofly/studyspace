#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "cond.h"
#include "threadpool.h"

void *threadpool_routine(void *arg);

// initialize thread pool
int threadpool_init(threadpool_t* pool, int max_threads) {
    if (pool == NULL) {
        fprintf(stderr, "Cannot init threadpool on NULL object!");
        return THREADPOOL_INIT_ERROR;
    }
    memset(pool, 0, sizeof(threadpool_t));
    pool->capacity = max_threads;

    int ret = 0;
    ret = pthread_cond_init(&(pool->ready.p_cond), NULL);
    if (ret != 0) {
        fprintf(stderr, "Cannot init threadpool on NULL object!");
        return THREADPOOL_INIT_ERROR;
    }

    ret = pthread_mutex_init(&(pool->ready.p_lock), NULL);
    if (ret != 0) {
        fprintf(stderr, "Cannot init threadpool on NULL object!");
        return THREADPOOL_INIT_ERROR;
    }
    return THREADPOOL_SUCCESS;
}


// deallocate thread pool
int threadpool_destroy(threadpool_t *pool) {
    if (pool == NULL) {
        fprintf(stderr, "Threadpool seems already destroyed.");
        return THREADPOOL_SUCCESS;
    }
    
    int ret = 0;
    ret = pthread_cond_destroy(&(pool->ready.p_cond));
    if (ret != 0) {
        fprintf(stderr, "Threadpool seems already destroyed.");
        return THREADPOOL_DESTROY_ERROR;
    }
    ret = pthread_mutex_destroy(&(pool->ready.p_lock));
    if (ret != 0) {
        fprintf(stderr, "Threadpool seems already destroyed.");
        return THREADPOOL_DESTROY_ERROR;
    }
    
    pool->quit = THREADPOOL_QUIT;
    return THREADPOOL_SUCCESS;
}


// add a task to thread pool
int threadpool_addtask(threadpool_t *pool, TASK_ROUTINE func, TASK_ARGUMENTS args) {
    task_t *ptask = (task_t *) malloc(sizeof(task_t));
    if (ptask == NULL) {
        fprintf(stderr, "No enough memory!");
        return THREADPOOL_MEM_ERROR;
    }
    ptask->func = func;
    ptask->args = args;
    ptask->next = NULL;

    cond_lock(&(pool->ready));
    if (pool->quit == THREADPOOL_QUIT) {
        fprintf(stderr, "Threadpool already quit!");
        free(ptask);
        return THREADPOOL_STATE_ERROR;
    }

    if (pool->head == NULL) {
        pool->head = ptask;
    } else {
        pool->tail->next = ptask;
    }
    pool->tail = ptask;

    if (pool->idle > 0) {
        cond_signal(&pool->ready);
    } else if (pool->count < pool->capacity) {
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, threadpool_routine, (void *)pool);
        if (ret != 0) {
            fprintf(stderr, "pthread_create");
        }
        pool->count ++;
    }
    cond_unlock(&pool->ready);
    
    return THREADPOOL_SUCCESS;
}


void *threadpool_routine(void *arg) {
    pthread_t tid = pthread_self();
    printf("Thread %#lx starting\n", (size_t) tid);
    threadpool_t *pool = (threadpool_t *) arg;
    long timedout;
    
    while (1) {
        timedout = 0;
        cond_lock(&pool->ready);

        pool->idle++;
        // waiting for new task or the destroy of thread pool
        while ((NULL == pool->head) && (THREADPOOL_QUIT == pool->quit)) {
            printf("Thread %#lx waiting\n", (size_t)tid);

            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + 2;
            ts.tv_nsec = 0;

            int ret = cond_timewait(&pool->ready, &ts);
            if (ETIMEDOUT == ret) {
                printf("Thread %#lx waiting timedout\n", (size_t)tid);
                timedout = 1;
                break;
            }
        }

        pool->idle--;
        if (pool->head != NULL) {
            task_t *tk = pool->head;
            pool->head = tk->next;
            cond_unlock(&pool->ready);
            tk->func(tk->args);
            free(tk);
            cond_lock(&pool->ready);
        }

        if (pool->quit == THREADPOOL_QUIT && NULL == pool->head) {
            pool->count--;
            if (0 == pool->count) {
                cond_signal(&pool->ready);
            }
            cond_unlock(&pool->ready);
            break;
        }

        // wait timedout
        if (timedout && NULL == pool->head) {
            pool->count--;
            cond_unlock(&pool->ready);
            break;
        }

        cond_unlock(&pool->ready);
    }
    
    printf("Thread %#lx exiting\n", (size_t)tid);
    return NULL;
}
