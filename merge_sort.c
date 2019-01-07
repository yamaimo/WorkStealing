#include <string.h>

#include "sort.h"

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

        // copy first_array in order to get merge space.
        int tmp_array[first_length];
        memcpy(tmp_array, first_array, sizeof(int) * first_length);

        int first_index = 0;
        int second_index = 0;
        int merge_index = 0;
        while (first_index < first_length)
        {
            if ((second_index >= second_length)
                || (tmp_array[first_index] <= second_array[second_index]))
            {
                array[merge_index] = tmp_array[first_index];
                first_index++;
            }
            else
            {
                array[merge_index] = second_array[second_index];
                second_index++;
            }
            merge_index++;
        }
    }
}

void sort(int* array, int length)
{
    merge_sort(array, length);
}
