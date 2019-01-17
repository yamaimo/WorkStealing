#pragma once

#include "task.h"

typedef struct thread_pool thread_pool_t;

thread_pool_t* thread_pool_create(int worker_size);

task_t* thread_pool_fork(thread_pool_t* thread_pool, task_entry_t entry, void* arg);
void thread_pool_join(thread_pool_t* thread_pool, task_t* task);

void thread_pool_delete(thread_pool_t* thread_pool);
