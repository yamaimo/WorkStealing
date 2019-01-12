#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "array.h"

static array_t* array_create(int length)
{
    array_t* array = (array_t*)malloc(sizeof(array_t));

    array->data = (int*)malloc(sizeof(int) * length);
    array->length = length;

    return array;
}

typedef int (*printer_t)(const char* format, ...);

int null_printer(const char* format, ...)
{
    // do nothing
    return 0;
}

static int read_int(FILE* input, const char* message)
{
    int ret = 0;
    printer_t printer = (input == stdin) ? printf : null_printer;

    while (true)
    {
        printer("%s", message);
        int scanned = fscanf(input, "%d", &ret);
        if (scanned == 0 || scanned == EOF)
        {
            fprintf(stderr, "read error.\n");
            fscanf(input, "%*s");   // flush error input
        }
        else
        {
            return ret;
        }
    }
}

array_t* array_from_input(FILE* input)
{
    int length = 0;
    while (true)
    {
        length = read_int(input, "input length: ");
        if (length <= 0)
        {
            fprintf(stderr, "length must be positive.\n");
        }
        else
        {
            break;
        }
    }

    array_t* array = array_create(length);

    const unsigned int buf_size = 32u;
    char buf[buf_size];
    for (int i = 0; i < length; i++)
    {
        snprintf(buf, buf_size, "[%d]: ", i);
        array->data[i] = read_int(input, buf);
    }

    return array;
}

array_t* array_copy(array_t* origin)
{
    array_t* copied = array_create(origin->length);

    memcpy(copied->data, origin->data, sizeof(int) * origin->length);
    copied->length = origin->length;

    return copied;
}

void array_print(array_t* array)
{
    for (int i = 0; i < array->length; i++)
    {
        printf("%d\n", array->data[i]);
    }
}

bool array_is_sorted(array_t* array)
{
    for (int i = 0; i < (array->length - 1); i++)
    {
        if (array->data[i] > array->data[i + 1])
        {
            return false;
        }
    }

    return true;
}

void array_delete(array_t* array)
{
    free(array->data);
    free(array);
}
