#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

#include "task.h"
#include "task_deque.h"
#include "thread_pool.h"

static const size_t stack_size_shift = 10u; // 500KB -> 500MB (Mac OS X)

typedef struct worker worker_t;
typedef struct worker_arg worker_arg_t;

struct worker
{
    pthread_t thread;
    bool exit;
};

struct worker_arg
{
    task_deque_t* fifo;
    bool* exit;
};

struct thread_pool
{
    task_deque_t* fifo;
    int worker_size;
    worker_t workers[]; /* variable array */
};

static void* worker_entry(void* arg)
{
    worker_arg_t* worker_arg = (worker_arg_t*)arg;

    while (! *worker_arg->exit)
    {
        task_t* task = task_deque_pop_front(worker_arg->fifo);
        if (task != NULL)
        {
            task_execute(task);
        }
        else
        {
            sched_yield();
        }
    }

    free(worker_arg);

    return NULL;
}

thread_pool_t* thread_pool_create(int worker_size)
{
    thread_pool_t* thread_pool = (thread_pool_t*)malloc(sizeof(thread_pool_t) + sizeof(worker_t) * worker_size);

    thread_pool->fifo = task_deque_create();
    thread_pool->worker_size = worker_size;
    for (int i = 0; i < worker_size; i++)
    {
        worker_arg_t* worker_arg = (worker_arg_t*)malloc(sizeof(worker_arg_t));

        worker_arg->fifo = thread_pool->fifo;
        worker_arg->exit = &thread_pool->workers[i].exit;
        *worker_arg->exit = false;

        /*
         * the stack size of the worker thread may be deep,
         * so make the stack size large.
         */
        pthread_attr_t attr;
        size_t stack_size;
        pthread_attr_init(&attr);
        pthread_attr_getstacksize(&attr, &stack_size);
        stack_size <<= stack_size_shift;
        pthread_attr_setstacksize(&attr, stack_size);

        pthread_create(&thread_pool->workers[i].thread, &attr, worker_entry, worker_arg);

        pthread_attr_destroy(&attr);
    }

    return thread_pool;
}

task_t* thread_pool_fork(thread_pool_t* thread_pool, task_entry_t entry, void* arg)
{
    task_t* task = task_create(entry, arg);
    task_deque_push_back(thread_pool->fifo, task);
    return task;
}

void thread_pool_join(thread_pool_t* thread_pool, task_t* task)
{
    while (!task->is_done)
    {
        task_t* fetched = task_deque_pop_front(thread_pool->fifo);
        if (fetched != NULL)
        {
            task_execute(fetched);
        }
        else
        {
            sched_yield();
        }
    }
}

void thread_pool_delete(thread_pool_t* thread_pool)
{
    for (int i = 0; i < thread_pool->worker_size; i++)
    {
        thread_pool->workers[i].exit = true;
    }
    for (int i = 0; i < thread_pool->worker_size; i++)
    {
        pthread_join(thread_pool->workers[i].thread, NULL);
    }

    free(thread_pool);
}
