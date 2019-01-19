all: merge_sort split_merge_sort fork_join_merge_sort post_fetch_merge_sort fifo_merge_sort work_stealing_merge_sort

merge_sort: main.o array.o merge_sort.o
	clang -o $@ $^

split_merge_sort: main.o array.o split_merge_sort.o
	clang -o $@ $^

fork_join_merge_sort: main.o array.o fork_join_merge_sort.o
	clang -o $@ $^

post_fetch_merge_sort: main.o array.o task.o task_deque.o post_fetch_merge_sort.o
	clang -o $@ $^

fifo_merge_sort: main.o array.o task.o task_deque.o fifo_thread_pool.o thread_pool_merge_sort.o
	clang -o $@ $^

work_stealing_merge_sort: main.o array.o task.o task_deque.o work_stealing_thread_pool.o thread_pool_merge_sort.o
	clang -o $@ $^

%.o: %.c
	clang -c $<

clean:
	-rm merge_sort split_merge_sort fork_join_merge_sort post_fetch_merge_sort fifo_merge_sort work_stealing_merge_sort *.o
