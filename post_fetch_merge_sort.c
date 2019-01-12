#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

#include "array.h"
#include "task.h"
#include "task_deque.h"

static const int fetch_thread_size = 4;

typedef struct post_thread_arg post_thread_arg_t;
typedef struct fetch_thread_arg fetch_thread_arg_t;
typedef struct merge_task_arg merge_task_arg_t;

struct post_thread_arg
{
    task_deque_t* task_deque;
    array_t* array;
    bool fetch_thread_exit[fetch_thread_size];
};

struct fetch_thread_arg
{
    task_deque_t* task_deque;
    bool* exit;
};

struct merge_task_arg
{
    task_t* sort_tasks[2];
};

static void* post_thread_entry(void* arg);
static void* fetch_thread_entry(void* arg);

static task_t* post_sort_task(task_deque_t* task_deque, array_t* array);
static task_t* post_merge_task(task_deque_t* task_deque, task_t* sort_tasks[2]);

static void* merge_task_entry(void* arg);

static void* post_thread_entry(void* arg)
{
    post_thread_arg_t* post_thread_arg = (post_thread_arg_t*)arg;

    task_t* sort_task = post_sort_task(post_thread_arg->task_deque, post_thread_arg->array);
    while (!sort_task->is_done)
    {
        sched_yield();
    }

    for (int i = 0; i < fetch_thread_size; i++)
    {
        post_thread_arg->fetch_thread_exit[i] = true;
    }

    return NULL;
}

static void* fetch_thread_entry(void* arg)
{
    fetch_thread_arg_t* fetch_thread_arg = (fetch_thread_arg_t*)arg;

    while (! *fetch_thread_arg->exit)
    {
        task_t* task = task_deque_pop_front(fetch_thread_arg->task_deque);
        if (task != NULL)
        {
            task_execute(task);
        }
        else
        {
            sched_yield();
        }
    }

    return NULL;
}

static task_t* post_sort_task(task_deque_t* task_deque, array_t* array)
{
    task_t* sort_task = NULL;

    if (array->length > 1)
    {
        task_t* sort_tasks[2];

        array_t first_array = {
            .data = array->data,
            .length = array->length / 2,
        };
        array_t second_array = {
            .data = array->data + first_array.length,
            .length = array->length - first_array.length,
        };

        sort_tasks[0] = post_sort_task(task_deque, &first_array);
        sort_tasks[1] = post_sort_task(task_deque, &second_array);

        sort_task = post_merge_task(task_deque, sort_tasks);
    }
    else
    {
        sort_task = task_create(NULL, NULL);

        array_t* sorted = (array_t*)malloc(sizeof(array_t));
        sorted->data = array->data;
        sorted->length = array->length;

        sort_task->ret = sorted;
        sort_task->is_done = true;
    }

    return sort_task;
}

static task_t* post_merge_task(task_deque_t* task_deque, task_t* sort_tasks[2])
{
    merge_task_arg_t* merge_task_arg = (merge_task_arg_t*)malloc(sizeof(merge_task_arg_t));

    merge_task_arg->sort_tasks[0] = sort_tasks[0];
    merge_task_arg->sort_tasks[1] = sort_tasks[1];

    task_t* merge_task = task_create(merge_task_entry, merge_task_arg);
    task_deque_push_back(task_deque, merge_task);

    return merge_task;
}

static void merge_array(array_t* first_array, array_t* second_array)
{
    // copy first_array in order to get merge space.
    array_t* tmp_array = array_copy(first_array);

    int first_index = 0;
    int second_index = 0;
    int merge_index = 0;
    while (first_index < first_array->length)
    {
        if ((second_index >= second_array->length)
            || (tmp_array->data[first_index] <= second_array->data[second_index]))
        {
            first_array->data[merge_index] = tmp_array->data[first_index];
            first_index++;
        }
        else
        {
            first_array->data[merge_index] = second_array->data[second_index];
            second_index++;
        }
        merge_index++;
    }

    array_delete(tmp_array);
}

static void* merge_task_entry(void* arg)
{
    merge_task_arg_t* merge_task_arg = (merge_task_arg_t*)arg;
    task_t** sort_tasks = merge_task_arg->sort_tasks;

    while ((!sort_tasks[0]->is_done) || (!sort_tasks[1]->is_done))
    {
        sched_yield();
    }

    array_t* first_array = (array_t*)sort_tasks[0]->ret;
    array_t* second_array = (array_t*)sort_tasks[1]->ret;

    merge_array(first_array, second_array);

    array_t* ret = (array_t*)malloc(sizeof(array_t));
    ret->data = first_array->data;
    ret->length = first_array->length + second_array->length;

    task_delete(sort_tasks[0]);
    task_delete(sort_tasks[1]);
    free(merge_task_arg);

    return ret;
}

void array_sort(array_t* array)
{
    task_deque_t* task_deque = task_deque_create();

    pthread_t post_thread;
    post_thread_arg_t post_thread_arg = {
        .task_deque = task_deque,
        .array = array,
    };

    pthread_t fetch_threads[fetch_thread_size];
    fetch_thread_arg_t fetch_thread_arg[fetch_thread_size];

    pthread_create(&post_thread, NULL, post_thread_entry, &post_thread_arg);
    for (int i = 0; i < fetch_thread_size; i++)
    {
        fetch_thread_arg[i].task_deque = task_deque;
        fetch_thread_arg[i].exit = &post_thread_arg.fetch_thread_exit[i];
        *fetch_thread_arg[i].exit = false;
        pthread_create(&fetch_threads[i], NULL, fetch_thread_entry, &fetch_thread_arg[i]);
    }

    pthread_join(post_thread, NULL);
    for (int i = 0; i < fetch_thread_size; i++)
    {
        pthread_join(fetch_threads[i], NULL);
    }

    task_deque_delete(task_deque);
}
