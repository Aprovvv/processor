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
    long reg[16];
};

static int pr (const void* p);
static void get_str(FILE* fp, char* str, int size);
static void dump(FILE* fp, const struct spu* proc, size_t ip);

int main()
{
    long cmd = 0;
    size_t ip = 0;
    struct spu proc= {};
    stderr = fopen("log.txt", "w");

    proc.stk = stack_init(sizeof(long), 4);
    proc.code = stack_init(sizeof(long), 4);
    proc.lbl_stack = stack_init(sizeof(struct lbl_t), 4);
    FILE* fp = fopen("cmds.txt", "r");

    while (fscanf(fp, "%ld", &cmd) != EOF)
    {
        //printf("cmd = %ld\n", cmd);
        stack_push(proc.code, &cmd);
        switch(cmd)
        {
            case PUSH:
            {
                fscanf(fp, "%ld", &cmd);
                stack_push(proc.code, &cmd);
                fscanf(fp, "%ld", &cmd);
                stack_push(proc.code, &cmd);
                ip += 2;
                break;
            }
            case LBL:
            {
                struct lbl_t l = {};
                get_str(fp, l.name, LBL_SIZE);
                //ip++;
                l.addr = (long)ip;
                stack_push(proc.lbl_stack, &l);
                fprintf(stderr, "l.name = %s; l.addr = %d\n", l.name, l.addr);
                break;
            }
            case JMP:
            case JA:
            case JAE:
            case JB:
            case JBE:
            case JE:
            case JME:
            {
                char name[LBL_SIZE] = "";
                struct lbl_t l = {};
                get_str(fp, name, LBL_SIZE);
                fprintf(stderr, "name = %s\n", name);
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
                fprintf(stderr, "addr = %ld\n", l.addr);
                break;
            }
            default:
                break;
        }
        ip++;
    }
    ip = 0;

    for(size_t i = 0; i < stack_size(proc.code); i++)
    {
        long x;
        stack_view(proc.code, i, &x);
        fprintf(stderr, "%ld ", x);
    }
    fputc('\n', stderr);

    for(ip = 0; ip < stack_size(proc.code); ip++)
    {
        dump(stderr, &proc, ip);
        long a, b;
        stack_view(proc.code, ip, &cmd);
        switch (cmd)
        {
            case PUSH:
            {
                long dest = 0;
                stack_view(proc.code, ++ip, &dest);
                stack_view(proc.code, ++ip, &a);
                if (dest & 1)
                    stack_push(proc.stk, &a);
                if (dest & 2)
                    stack_push(proc.stk, &proc.reg[a]);
                break;
            }
            case POP:
            {
                long dest = 0;
                stack_view(proc.code, ++ip, &dest);
                stack_pop(proc.stk, &proc.reg[dest]);
                break;
            }
            case SUM:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = b + a;
                stack_push(proc.stk, &a);
                break;
            case SUB:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = b - a;
                stack_push(proc.stk, &a);
                break;
            case MULT:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = b*a;
                stack_push(proc.stk, &a);
                break;
            case DIV:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = b/a;
                stack_push(proc.stk, &a);
                break;
            case OUT:
                stack_pop(proc.stk, &a);
                fprintf(stderr, "%ld\n", a);
                break;
            case HLT:
                goto stop_reading;
                break;
            case LBL:
                break;
            case JMP:
            {
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                ip = (size_t)addr;
                //abort();
                break;
            }
            case JA:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                if (b > a)
                    ip = (size_t)addr;
                break;
            }
            case JAE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                if (b >= a)
                    ip = (size_t)addr;
                break;
            }
            case JB:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                if (b < a)
                    ip = (size_t)addr;
                break;
            }
            case JBE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                if (b <= a)
                    ip = (size_t)addr;
                break;
            }
            case JE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                if (b == a)
                    ip = (size_t)addr;
                break;
            }
            case JME:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                long addr = (long)++ip;
                stack_view(proc.code, ip, &addr);
                if (b != a)
                    ip = (size_t)addr;
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

static void dump(FILE* fp, const struct spu* proc, size_t ip)
{
    fprintf(fp, "------------------------------------\n");
    fprintf(fp, "code ");
    for (size_t i = 0; i < stack_size(proc->code); i++)
        fprintf(fp, "%2X ", i);
    fprintf(fp, "\n     ");
    for (size_t i = 0; i < stack_size(proc->code); i++)
    {
        long x = 0;
        stack_view(proc->code, i, &x);
        fprintf(fp, "%2X ", x);
    }
    fprintf(fp, "\n      ");
    for (int i = 0; i < ip; i++)
        fprintf(fp, "~~~");
    fprintf(fp, "^~ip=%d\n", ip);
    fprintf(fp, "------------------------------------\n\n");
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
