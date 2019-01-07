#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "util.h"

static int read_int(const char* message)
{
    int ret = 0;

    while (true)
    {
        printf("%s", message);
        int scanned = scanf("%d", &ret);
        if (scanned == 0 || scanned == EOF)
        {
            printf("read error.\n");
            scanf("%*s");   // flush error input
        }
        else
        {
            return ret;
        }
    }
}

void print_array(int* array, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%d\n", array[i]);
    }
}

void read_array(int** p_array, int* p_length)
{
    int length = 0;
    while (true)
    {
        length = *p_length = read_int("input length: ");
        if (length <= 0)
        {
            printf("length must be positive.\n");
        }
        else
        {
            break;
        }
    }

    int* array = *p_array = (int*)malloc(sizeof(int) * length);

    const unsigned int buf_size = 32u;
    char buf[buf_size];
    for (int i = 0; i < length; i++)
    {
        snprintf(buf, buf_size, "[%d]: ", i);
        array[i] = read_int(buf);
    }
}

bool check_sorted(int* array, int length)
{
    for (int i = 0; i < (length - 1); i++)
    {
        if (array[i] > array[i + 1])
        {
            return false;
        }
    }

    return true;
}
