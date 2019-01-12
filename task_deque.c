#include <stdlib.h>
#include <pthread.h>

#include "task.h"
#include "task_deque.h"

// task link

typedef struct task_link task_link_t;

struct task_link
{
    task_link_t* prev;
    task_link_t* next;
    task_t* task;
};

static task_link_t* task_link_create(task_t* task)
{
    task_link_t* task_link = (task_link_t*)malloc(sizeof(task_link_t));

    task_link->prev = NULL;
    task_link->next = NULL;
    task_link->task = task;

    return task_link;
}

static void task_link_delete(task_link_t* task_link)
{
    free(task_link);
}

// task deque

struct task_deque
{
    task_link_t* front;
    task_link_t* back;
    pthread_mutex_t mutex;
};

task_deque_t* task_deque_create(void)
{
    task_deque_t* task_deque = (task_deque_t*)malloc(sizeof(task_deque_t));

    task_deque->front = NULL;
    task_deque->back = NULL;
    pthread_mutex_init(&task_deque->mutex, NULL);

    return task_deque;
}

void task_deque_push_front(task_deque_t* task_deque, task_t* task)
{
    task_link_t* task_link = task_link_create(task);

    pthread_mutex_lock(&task_deque->mutex);
    {
        if (task_deque->front == NULL)
        {
            task_deque->front = task_link;
            task_deque->back = task_link;
        }
        else
        {
            task_deque->front->prev = task_link;
            task_link->next = task_deque->front;
            task_deque->front = task_link;
        }
    }
    pthread_mutex_unlock(&task_deque->mutex);
}

void task_deque_push_back(task_deque_t* task_deque, task_t* task)
{
    task_link_t* task_link = task_link_create(task);

    pthread_mutex_lock(&task_deque->mutex);
    {
        if (task_deque->back == NULL)
        {
            task_deque->front = task_link;
            task_deque->back = task_link;
        }
        else
        {
            task_deque->back->next = task_link;
            task_link->prev = task_deque->back;
            task_deque->back = task_link;
        }
    }
    pthread_mutex_unlock(&task_deque->mutex);
}

task_t* task_deque_pop_front(task_deque_t* task_deque)
{
    task_t* task = NULL;
    task_link_t* task_link = NULL;

    pthread_mutex_lock(&task_deque->mutex);
    {
        if (task_deque->front != NULL)
        {
            task_link = task_deque->front;
            if (task_link->next != NULL)
            {
                task_deque->front = task_link->next;
                task_deque->front->prev = NULL;
            }
            else
            {
                task_deque->front = NULL;
                task_deque->back = NULL;
            }
        }
    }
    pthread_mutex_unlock(&task_deque->mutex);

    if (task_link != NULL)
    {
        task = task_link->task;
        task_link_delete(task_link);
    }

    return task;
}

task_t* task_deque_pop_back(task_deque_t* task_deque)
{
    task_t* task = NULL;
    task_link_t* task_link = NULL;

    pthread_mutex_lock(&task_deque->mutex);
    {
        if (task_deque->back != NULL)
        {
            task_link = task_deque->back;
            if (task_link->prev != NULL)
            {
                task_deque->back = task_link->prev;
                task_deque->back->next = NULL;
            }
            else
            {
                task_deque->front = NULL;
                task_deque->back = NULL;
            }
        }
    }
    pthread_mutex_unlock(&task_deque->mutex);

    if (task_link != NULL)
    {
        task = task_link->task;
        task_link_delete(task_link);
    }

    return task;
}

void task_deque_delete(task_deque_t* task_deque)
{
    pthread_mutex_destroy(&task_deque->mutex);
    free(task_deque);
}
