#include <stdlib.h>

#include "array.h"
#include "task.h"
#include "thread_pool.h"

#define REDUCE_THREAD_STACK 0

static const int thread_pool_size = 31;

typedef struct sort_task_arg sort_task_arg_t;

struct sort_task_arg
{
    thread_pool_t* thread_pool;
    array_t* array;
};

#if REDUCE_THREAD_STACK
struct stack_data
{
    array_t first_array;
    array_t second_array;
    sort_task_arg_t first_task_arg;
    sort_task_arg_t second_task_arg;
    task_t* forked_task;
};
typedef struct stack_data stack_data_t;
#endif

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

static void* sort_task(void* arg)
{
    sort_task_arg_t* sort_task_arg = (sort_task_arg_t*)arg;

    thread_pool_t* thread_pool = sort_task_arg->thread_pool;
    array_t* array = sort_task_arg->array;

    if (array->length > 1)
    {
#if REDUCE_THREAD_STACK
        stack_data_t* stack_data = (stack_data_t*)malloc(sizeof(stack_data_t));

        stack_data->first_array.data = array->data;
        stack_data->first_array.length = array->length / 2;
        stack_data->second_array.data = array->data + stack_data->first_array.length;
        stack_data->second_array.length = array->length - stack_data->first_array.length;

        stack_data->first_task_arg.thread_pool = thread_pool;
        stack_data->first_task_arg.array = &stack_data->first_array;
        stack_data->second_task_arg.thread_pool = thread_pool;
        stack_data->second_task_arg.array = &stack_data->second_array;

        stack_data->forked_task = thread_pool_fork(thread_pool, sort_task, &stack_data->first_task_arg);
        sort_task(&stack_data->second_task_arg);

        thread_pool_join(thread_pool, stack_data->forked_task);
        task_delete(stack_data->forked_task);

        merge_array(&stack_data->first_array, &stack_data->second_array);

        free(stack_data);
#else
        array_t first_array = {
            .data = array->data,
            .length = array->length / 2,
        };
        array_t second_array = {
            .data = array->data + first_array.length,
            .length = array->length - first_array.length,
        };

        sort_task_arg_t first_task_arg = {
            .thread_pool = thread_pool,
            .array = &first_array,
        };
        sort_task_arg_t second_task_arg = {
            .thread_pool = thread_pool,
            .array = &second_array,
        };

        task_t* forked = thread_pool_fork(thread_pool, sort_task, &first_task_arg);
        sort_task(&second_task_arg);

        thread_pool_join(thread_pool, forked);
        task_delete(forked);

        merge_array(&first_array, &second_array);
#endif
    }

    return NULL;
}

void array_sort(array_t* array)
{
    thread_pool_t* thread_pool = thread_pool_create(thread_pool_size);

    sort_task_arg_t sort_task_arg = {
        .thread_pool = thread_pool,
        .array = array,
    };
    sort_task(&sort_task_arg);

    thread_pool_delete(thread_pool);
}
