#ifndef THREADPOOL_T
#define THREADPOOL_T
#include "cond.h"

#define THREADPOOL_SUCCESS          0
#define THREADPOOL_INIT_ERROR       1
#define THREADPOOL_DESTROY_ERROR    2
#define THREADPOOL_STATE_ERROR      3
#define THREADPOOL_MEM_ERROR        4

#define THREADPOOL_READY            0
#define THREADPOOL_QUIT             1

typedef void* (*TASK_ROUTINE) (void*);
typedef void* TASK_ARGUMENTS;

typedef struct task {
    TASK_ROUTINE    func;   // task handler
    TASK_ARGUMENTS  args;   // parameter for handler run
    struct task     *next;  // pointer to the next task
} task_t;


typedef struct threadpool {
    cond_t      ready;      // for controll
    task_t      *head;      // head of task-linklist
    task_t      *tail;      // tail of task-linklist
    int         count;      // count of thread at present
    int         idle;       // idle thread count at present
    int         capacity;   // max threads for this thread pool
    int         quit;       // set 1 when destroying thread pool
} threadpool_t;

// initialize thread pool
int threadpool_init(threadpool_t* pool, int max_threads);

// deallocate thread pool
int threadpool_destroy(threadpool_t *pool);

// add a task to thread pool
int threadpool_addtask(threadpool_t *pool, TASK_ROUTINE func, TASK_ARGUMENTS args);

#endif
