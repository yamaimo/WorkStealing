#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef struct array array_t;

struct array
{
    int* data;
    int length;
};

array_t* array_from_input(FILE* input);
array_t* array_copy(array_t* origin);

void array_print(array_t* array);
bool array_is_sorted(array_t* array);

void array_sort(array_t* array);

void array_delete(array_t* array);
