#include "array.h"

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

void array_sort(array_t* array)
{
    merge_sort(array);
}
