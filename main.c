#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include "util.h"
#include "sort.h"

int main(int argc, char** argv)
{
    int* array;
    int length;

    read_array(&array, &length);
    printf("length is %d\n", length);
    print_array(array, length);

    struct timeval begin;
    struct timeval end;

    gettimeofday(&begin, NULL);
    sort(array, length);
    gettimeofday(&end, NULL);

    const char* result = check_sorted(array, length)
        ? "sorted."
        : "not sorted.";
    printf("%s\n", result);
    print_array(array, length);

    double elapsed = (double)(end.tv_sec - begin.tv_sec);
    elapsed += (double)(end.tv_usec - begin.tv_usec) / (1000.0 * 1000.0);
    printf("elapsed: %.3lf [msec]\n", elapsed);

    free(array);
    return 0;
}
