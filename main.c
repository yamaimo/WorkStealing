#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include "array.h"

int main(int argc, char** argv)
{
    // read array

    FILE* input = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (input == NULL)
    {
        fprintf(stderr, "failed to open file %s.", argv[1]);
        exit(1);
    }

    array_t* array = array_from_input(input);

    if (input != stdin)
    {
        fclose(input);
    }

    printf("length is %d\n", array->length);
    array_print(array);

    // sort

    struct timeval begin;
    struct timeval end;

    gettimeofday(&begin, NULL);
    array_sort(array);
    gettimeofday(&end, NULL);

    const char* result = array_is_sorted(array) ? "sorted." : "not sorted.";
    double elapsed = (double)(end.tv_sec - begin.tv_sec);
    elapsed += (double)(end.tv_usec - begin.tv_usec) / (1000.0 * 1000.0);

    // output result

    printf("%s\n", result);
    array_print(array);
    printf("elapsed: %.3lf [msec]\n", elapsed);

    array_delete(array);
    return 0;
}
