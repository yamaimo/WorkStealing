#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "sort.h"

static void merge_sort(int* array, int length);

struct thread_arg
{
    int* array;
    int length;
};
typedef struct thread_arg thread_arg_t;

static void* thread_entry(void* arg)
{
    thread_arg_t* thread_arg = (thread_arg_t*)arg;
    merge_sort(thread_arg->array, thread_arg->length);
    return NULL;
}

static void merge_array(int* first_array, int first_length, int* second_array, int second_length)
{
    // copy first_array in order to get merge space.
    // NOTE: if first_length is too large, thread stack will over.
    int* tmp_array = (int*)malloc(sizeof(int) * first_length);
    memcpy(tmp_array, first_array, sizeof(int) * first_length);

    int first_index = 0;
    int second_index = 0;
    int merge_index = 0;
    while (first_index < first_length)
    {
        if ((second_index >= second_length)
            || (tmp_array[first_index] <= second_array[second_index]))
        {
            first_array[merge_index] = tmp_array[first_index];
            first_index++;
        }
        else
        {
            first_array[merge_index] = second_array[second_index];
            second_index++;
        }
        merge_index++;
    }

    free(tmp_array);
}

static void merge_sort(int* array, int length)
{
    if (length > 1)
    {
        int* first_array = array;
        int first_length = length / 2;
        int* second_array = array + first_length;
        int second_length = length - first_length;

        // fork and  sort first_array in other thread.
        pthread_t thread;
        thread_arg_t thread_arg = {
            .array = first_array,
            .length = first_length,
        };
        int error = pthread_create(&thread, NULL, thread_entry, &thread_arg);

        if (!error)
        {
            // sort second_array in this thread.
            merge_sort(second_array, second_length);

            // join
            pthread_join(thread, NULL);
        }
        else
        {
            // fork failed.
            // sort both first_array and second_array in this thread.
            merge_sort(first_array, first_length);
            merge_sort(second_array, second_length);
        }

        merge_array(first_array, first_length, second_array, second_length);
    }
}

void sort(int* array, int length)
{
    merge_sort(array, length);
}
