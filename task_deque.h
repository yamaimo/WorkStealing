#pragma once

#include "task.h"

typedef struct task_deque task_deque_t;

task_deque_t* task_deque_create(void);

void task_deque_push_front(task_deque_t* task_deque, task_t* task);
void task_deque_push_back(task_deque_t* task_deque, task_t* task);
task_t* task_deque_pop_front(task_deque_t* task_deque);
task_t* task_deque_pop_back(task_deque_t* task_deque);

void task_deque_delete(task_deque_t* task_deque);
