#include <stdlib.h>
#include <time.h>
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
    task_deque_t* deque;
    bool exit;
};

struct worker_arg
{
    thread_pool_t* thread_pool;
    task_deque_t* deque;
    bool* exit;
};

struct thread_pool
{
    pthread_key_t deque_key;    // to obtain the deque of the current thread
    int worker_size;
    worker_t workers[]; /* variable array */
};

static task_t* thread_pool_steal_task(thread_pool_t* thread_pool)
{
    task_t* stolen = NULL;

    int start = random() % thread_pool->worker_size;
    for (int i = 0; (i < thread_pool->worker_size) && (stolen == NULL); i++)
    {
        int index = (start + i) % thread_pool->worker_size;
        task_deque_t* worker_deque = thread_pool->workers[index].deque;
        stolen = task_deque_pop_front(worker_deque);
    }

    return stolen;
}

static void* worker_entry(void* arg)
{
    worker_arg_t* worker_arg = (worker_arg_t*)arg;

    pthread_setspecific(worker_arg->thread_pool->deque_key, worker_arg->deque);

    while (! *worker_arg->exit)
    {
        task_t* task = task_deque_pop_back(worker_arg->deque);
        if (task == NULL)
        {
            task = thread_pool_steal_task(worker_arg->thread_pool);
        }

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
    srandom((unsigned int)time(NULL));

    thread_pool_t* thread_pool = (thread_pool_t*)malloc(sizeof(thread_pool_t) + sizeof(worker_t) * worker_size);

    pthread_key_create(&thread_pool->deque_key, NULL);
    thread_pool->worker_size = worker_size;
    // create worker's deque before creating threads
    // because one worker may touch other worker's deque.
    for (int i = 0; i < worker_size; i++)
    {
        thread_pool->workers[i].deque = task_deque_create();
    }

    for (int i = 0; i < worker_size; i++)
    {
        worker_arg_t* worker_arg = (worker_arg_t*)malloc(sizeof(worker_arg_t));

        worker_arg->thread_pool = thread_pool;
        worker_arg->deque = thread_pool->workers[i].deque;
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

    task_deque_t* current_deque = (task_deque_t*)pthread_getspecific(thread_pool->deque_key);
    if (current_deque == NULL)
    {
        int index = random() % thread_pool->worker_size;
        current_deque = thread_pool->workers[index].deque;
    }

    task_deque_push_back(current_deque, task);

    return task;
}

void thread_pool_join(thread_pool_t* thread_pool, task_t* task)
{
    task_deque_t* current_deque = (task_deque_t*)pthread_getspecific(thread_pool->deque_key);
    if (current_deque == NULL)
    {
        int index = random() % thread_pool->worker_size;
        current_deque = thread_pool->workers[index].deque;
    }

    while (!task->is_done)
    {
        task_t* fetched = task_deque_pop_back(current_deque);
        if (fetched == NULL)
        {
            fetched = thread_pool_steal_task(thread_pool);
        }

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

    for (int i = 0; i < thread_pool->worker_size; i++)
    {
        task_deque_delete(thread_pool->workers[i].deque);
    }
    pthread_key_delete(thread_pool->deque_key);
    free(thread_pool);
}
