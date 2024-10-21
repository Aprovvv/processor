#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack/stack.h"
#include "cmds.h"

struct darr {
    size_t capacity;
    size_t size;
    long* ptr;
};

struct spu {
    struct stack_t* stk;
    int ip;
    struct darr code;
};

static int pr (const void* p);
static int darr_init(struct darr* str_ptr, size_t start_capacity);
static int darr_add(struct darr* str_ptr, long val);

int main()
{
    long cmd = 0;
    size_t ip = 0;

    struct spu proc= {};
    proc.stk = stack_init(sizeof(int), 4);
    FILE* fp = fopen("cmds.txt", "r");
    darr_init(&proc.code, 4);
    while (fscanf(fp, "%ld", &cmd) != EOF)
    {
        darr_add(&proc.code, cmd);
    }
    /*for(size_t i = 0; i < proc.code.capacity; i++)
    {
        printf("%ld ", proc.code.ptr[i]);
    }*/

    for(ip = 0; ip < proc.code.size; ip++)
    {
        long a, b;
        cmd = proc.code.ptr[ip];
        //printf("cmd = %ld\n", cmd);
        //stack_printf(stk, pr);
        switch (cmd)
        {
            case push:
                a = proc.code.ptr[++ip];
                stack_push(proc.stk, &a);
                break;
            case sum:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = a + b;
                stack_push(proc.stk, &a);
                break;
            case sub:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = b - a;
                stack_push(proc.stk, &a);
                break;
            case mult:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = a*b;
                stack_push(proc.stk, &a);
                break;
            case divv:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = b/a;
                stack_push(proc.stk, &a);
                break;
            case out:
                stack_pop(proc.stk, &a);
                printf("%ld\n", a);
                break;
            case hlt:
                goto stop_reading;
                break;
            default:
                fprintf(stderr, "SNTXERR %ld\n", cmd);
                goto stop_reading;
                break;
        }
    }
stop_reading:
    free(proc.code.ptr);
    stack_destroy(proc.stk);
}

static int pr (const void* p)
{
    const int* pi = (const int*)p;
    fprintf(stderr, "%d", *pi);
    return 0;
}

static int darr_init(struct darr* str_ptr, size_t start_capacity)
{
    str_ptr->ptr = (long*)calloc(sizeof(long), start_capacity);
    if (str_ptr == NULL)
        return 1;
    str_ptr->capacity = start_capacity;
    str_ptr->size = 0;
    return 0;
}

static int darr_add(struct darr* str_ptr, long val)
{
    if (str_ptr->capacity <= str_ptr->size)
    {
        long* temp_ptr = (long*)realloc(str_ptr->ptr, sizeof(long)*str_ptr->capacity*2);
        if (temp_ptr == NULL)
            return 1;
        str_ptr->capacity *= 2;
        str_ptr->ptr = temp_ptr;
    }
    //fprintf(stderr, "size = %zu cap = %zu\n", str_ptr->size, str_ptr->capacity);
    str_ptr->ptr[str_ptr->size++] = val;
    //fprintf(stderr, "%ld size = %zu cap = %zu\n", str_ptr->ptr[str_ptr->size-1], str_ptr->size, str_ptr->capacity);
    return 0;
}
