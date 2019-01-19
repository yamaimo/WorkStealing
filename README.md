# Merge Sort Demo with Work Stealing Algorithm

This repository provides merge sort demo with work stealing algorithm.

## How to Build

Use `make`.

## Programs

### merge_sort

A simple merge sort using only main thread.

### split_merge_sort

A merge sort which splits array for each worker thread at first 
and merges split arrays after each worker thread sorted its array.

### post_fetch_merge_sort

A merge sort using producer-consumer: a producer thread creates tasks 
and post them into the task deque, and each consumer thread fetches 
a task from the task deque and executes the task.

### fork_join_merge_sort

A merge sort using fork-join: one thread creates another thread (fork), 
the created thread sorts half of the array, and the original thread 
sorts the rest of array.
The original thread then waits for the created thread (join) 
and merges the sorted arrays.

This program may not work correctly for large array because too many 
threads will be created.
To solve this issue, work stealing algorithm is required.

### fifo_merge_sort

A merge sort using a thread pool which has a FIFO task queue.
When forking a task, the task is pushed into the bottom of the task queue.
Any idle worker thread, which has no task or waits for other task, 
fetches a task from the top of the task queue.

Current merge sort implementation uses recursive calls, 
so the stack size of the worker thread may be too small.
In such case, make the stack size or the number of workers large.

### work_stealing_merge_sort

A merge sort using a thread pool where each worker has its own task deque.
When forking a task, the task is pushed into the bottom of the worker's deque.
Any idle worker thread, which has no task or waits for other task, 
fetches a task from the bottom of the worker's deque.
If the worker's deque is empty, the worker will steal a task from the top of 
other worker's deque.

Current merge sort implementation uses recursive calls, 
so the stack size of the worker thread may be too small.
In such case, make the stack size or the number of workers large.

### make_data.rb

A Ruby script to create input data.
