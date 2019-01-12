#pragma once

#include <stdbool.h>

typedef void* (*task_entry_t)(void* arg);

struct task
{
    task_entry_t entry;
    void* arg;
    void* ret;
    bool is_done;
};
typedef struct task task_t;

task_t* task_create(task_entry_t entry, void* arg);

void* task_execute(task_t* task);

void task_delete(task_t* task);
