all: merge_sort split_merge_sort fork_join_merge_sort

merge_sort: main.o util.o merge_sort.o
	clang -o $@ $^

split_merge_sort: main.o util.o split_merge_sort.o
	clang -o $@ $^

fork_join_merge_sort: main.o util.o fork_join_merge_sort.o
	clang -o $@ $^

%.o: %.c
	clang -c $<

clean:
	-rm merge_sort split_merge_sort fork_join_merge_sort *.o
