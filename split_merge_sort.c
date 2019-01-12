#include <pthread.h>

#include "array.h"

static const int split_size = 8;

static void merge_array(array_t* first_array, array_t* second_array)
{
    // copy first_array in order to get merge space.
    array_t* tmp_array = array_copy(first_array);

    int first_index = 0;
    int second_index = 0;
    int merge_index = 0;
    while (first_index < first_array->length)
    {
        if ((second_index >= second_array->length)
            || (tmp_array->data[first_index] <= second_array->data[second_index]))
        {
            first_array->data[merge_index] = tmp_array->data[first_index];
            first_index++;
        }
        else
        {
            first_array->data[merge_index] = second_array->data[second_index];
            second_index++;
        }
        merge_index++;
    }

    array_delete(tmp_array);
}

static void merge_sort(array_t* array)
{
    if (array->length > 1)
    {
        array_t first_array = {
            .data = array->data,
            .length = array->length / 2,
        };
        array_t second_array = {
            .data = array->data + first_array.length,
            .length = array->length - first_array.length,
        };

        merge_sort(&first_array);
        merge_sort(&second_array);

        merge_array(&first_array, &second_array);
    }
}

static void* sort_thread(void* array)
{
    merge_sort((array_t*)array);
    return NULL;
}

void array_sort(array_t* array)
{
    pthread_t threads[split_size];
    array_t split_arrays[split_size];

    int split_length = array->length / split_size;
    int rest = array->length % split_size;

    int offset = 0;
    for (int i = 0; i < split_size; i++)
    {
        split_arrays[i].data = array->data + offset;
        split_arrays[i].length = split_length + ((i < rest) ? 1 : 0);
        offset += split_arrays[i].length;
        pthread_create(&threads[i], NULL, sort_thread, &split_arrays[i]);
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
    for (int next_index = 1; split_arrays[0].length != array->length; next_index *= 2)
    {
        for (int i = 0; (i + next_index) < split_size; i += next_index * 2)
        {
            merge_array(
                &split_arrays[i],
                &split_arrays[i + next_index]);
            split_arrays[i].length += split_arrays[i + next_index].length;
        }
    }
}
