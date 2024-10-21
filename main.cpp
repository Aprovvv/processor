#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack/stack.h"
#include "cmds.h"

struct lbl_t {
    long addr;
    char name[LBL_SIZE];
};

struct spu {
    struct stack_t* stk;
    int ip;
    struct stack_t* code;
    struct stack_t* lbl_stack;
};

static int pr (const void* p);
static void get_str(FILE* fp, char* str, int size);

int main()
{
    long cmd = 0;
    size_t ip = 0;

    struct spu proc= {};
    proc.stk = stack_init(sizeof(int), 4);
    proc.code = stack_init(sizeof(int), 4);
    proc.lbl_stack = stack_init(sizeof(struct lbl_t), 4);
    FILE* fp = fopen("cmds.txt", "r");
    while (fscanf(fp, "%ld", &cmd) != EOF)
    {
        printf("cmd = %d\n", cmd);
        stack_push(proc.code, &cmd);
        if (cmd == lbl)
        {
            struct lbl_t l = {};
            get_str(fp, l.name, LBL_SIZE);
            l.addr = (long)ip;
            stack_push(proc.lbl_stack, &l);
        }
        else if (cmd == jmp)
        {
            char name[LBL_SIZE] = "";
            struct lbl_t l = {};
            get_str(fp, name, LBL_SIZE);
            printf("name = %s\n", name);
            for (size_t i = 0; i < stack_size(proc.lbl_stack); i++)
            {
                stack_view(proc.lbl_stack, i, &l);
                if (strcmp(name, l.name) == 0)
                {
                    stack_push(proc.code, &l.addr);
                    ip++;
                    break;
                }
            }
        }
        ip++;
    }
    ip = 0;
    for(size_t i = 0; i < stack_size(proc.code); i++)
    {
        long x;
        stack_view(proc.code, i, &x);
        printf("%ld ", x);
    }
    putchar('\n');

    for(ip = 0; ip < stack_size(proc.code); ip++)
    {
        long a, b;
        stack_view(proc.code, ip, &cmd);
        //printf("cmd = %ld\n", cmd);
        //stack_printf(proc.stk, pr);
        switch (cmd)
        {
            case push:
                stack_view(proc.code, ++ip, &a);
                //printf("a = %ld\n", a);
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
            case lbl:
                break;
            case jmp:
            {
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                ip = (size_t)addr;
                //abort();
                break;
            }
            default:
                fprintf(stderr, "SNTXERR %ld\n", cmd);
                goto stop_reading;
                break;
        }
    }
stop_reading:
    stack_destroy(proc.lbl_stack);
    stack_destroy(proc.code);
    stack_destroy(proc.stk);
}

static int pr (const void* p)
{
    const long* pi = (const long*)p;
    fprintf(stderr, "%ld", *pi);
    return 0;
}

static void get_str(FILE* fp, char* str, int size)
{
    int i = 0, ch = 0;
    fgetc(fp);
    while (i < size)
    {
        ch = fgetc(fp);
        //putchar(ch);
        if (ch == 0 || isspace(ch))
            break;
        str[i] = ch;
        i++;
    }
    str[i] = 0;
}
