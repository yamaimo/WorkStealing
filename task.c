#include <stdlib.h>
#include <stdbool.h>

#include "task.h"

task_t* task_create(task_entry_t entry, void* arg)
{
    task_t* task = (task_t*)malloc(sizeof(task_t));

    task->entry = entry;
    task->arg = arg;
    task->ret = NULL;
    task->is_done = false;

    return task;
}

void* task_execute(task_t* task)
{
    void* ret = task->entry(task->arg);

    task->ret = ret;
    task->is_done = true;

    return ret;
}

void task_delete(task_t* task)
{
    if (task->ret != NULL)
    {
        free(task->ret);
    }
    free(task);
}
