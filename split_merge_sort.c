#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "sort.h"

static const int split_size = 8;
//static const int split_size = 1;

struct thread_arg
{
    int* array;
    int length;
};
typedef struct thread_arg thread_arg_t;

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

        merge_sort(first_array, first_length);
        merge_sort(second_array, second_length);

        merge_array(first_array, first_length, second_array, second_length);
    }
}

static void* thread_entry(void* arg)
{
    thread_arg_t* thread_arg = (thread_arg_t*)arg;
    merge_sort(thread_arg->array, thread_arg->length);
    return NULL;
}

void sort(int* array, int length)
{
    pthread_t threads[split_size];
    thread_arg_t thread_args[split_size];

    int split_length = length / split_size;
    int rest = length % split_size;

    int offset = 0;
    for (int i = 0; i < split_size; i++)
    {
        thread_args[i].array = array + offset;
        thread_args[i].length = split_length + ((i < rest) ? 1 : 0);
        offset += thread_args[i].length;
        pthread_create(&threads[i], NULL, thread_entry, &thread_args[i]);
    }

    for (int i = 0; i < split_size; i++)
    {
        pthread_join(threads[i], NULL);
    }

    /*
     * merge from bottom to up:
     *
     * next_index = 1
     * 
     *    i i+next_index
     *    V V
     *   |0|1|2|3|4|5|6|7| ... |split_size-1|
     *
     *        i i+next_index
     *        V V
     *   |0,1|2|3|4|5|6|7| ... |split_size-1|
     *
     *   ...
     *
     * next_index = 2
     *
     *    i   i+next_index
     *    V   V
     *   |0,1|2,3|4,5|6,7| ... |split_size-1|
     *
     *            i   i+next_index
     *            V   V
     *   |0,1|2,3|4,5|6,7| ... |split_size-1|
     *
     *   ...
     */
    for (int next_index = 1; thread_args[0].length != length; next_index *= 2)
    {
        for (int i = 0; (i + next_index) < split_size; i += next_index * 2)
        {
            merge_array(
                thread_args[i].array,
                thread_args[i].length,
                thread_args[i + next_index].array,
                thread_args[i + next_index].length);
            thread_args[i].length += thread_args[i + next_index].length;
        }
    }
}
