cmake_minimum_required(VERSION 2.6)
project(work_stealing)

add_executable(merge_sort
    main.c
    array.c
    merge_sort.c)

add_executable(split_merge_sort
    main.c
    array.c
    split_merge_sort.c)

add_executable(fork_join_merge_sort
    main.c
    array.c
    fork_join_merge_sort.c)

add_executable(post_fetch_merge_sort
    main.c
    array.c
    task.c
    task_deque.c
    post_fetch_merge_sort.c)

add_executable(fifo_merge_sort
    main.c
    array.c
    task.c
    task_deque.c
    fifo_thread_pool.c
    thread_pool_merge_sort.c)

add_executable(work_stealing_merge_sort
    main.c
    array.c
    task.c
    task_deque.c
    work_stealing_thread_pool.c
    thread_pool_merge_sort.c)
