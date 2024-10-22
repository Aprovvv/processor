#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "stack/stack.h"
#include "cmds.h"

struct spu {
    struct stack_t* stk;
    int ip;
    proc_elem_t* code;
    size_t code_size;
    proc_elem_t reg[4];
};

static int pr (const void* p);
static void dump(FILE* fp, const struct spu* proc, size_t ip);

int main()
{
    proc_elem_t cmd = 0;
    size_t ip = 0;
    struct spu proc= {};
    struct stat filedata = {};
    stderr = fopen("log.txt", "w");

    FILE* fp = fopen("cmds.bin", "rb");
    stat("cmds.bin", &filedata);
    proc.code = (proc_elem_t*)calloc(sizeof(proc_elem_t), (size_t)filedata.st_size);
    proc.code_size = (size_t)filedata.st_size/sizeof(proc_elem_t);
    fread(proc.code, 1, (size_t)filedata.st_size, fp);
    proc.stk = stack_init(sizeof(proc_elem_t), 4);

    for(size_t i = 0; i < proc.code_size; i++)
    {
        fprintf(stderr, "%ld ", proc.code[i]);
    }
    fputc('\n', stderr);

    for(ip = 0; ip < proc.code_size; ip++)
    {
        //dump(stderr, &proc, ip);
        proc_elem_t a, b;
        cmd = proc.code[ip];
        switch (cmd)
        {
            case PUSH:
            {
                proc_elem_t dest = proc.code[++ip];
                a = proc.code[++ip];
                if (dest & 1)
                    stack_push(proc.stk, &a);
                if (dest & 2)
                    stack_push(proc.stk, &proc.reg[a]);
                break;
            }
            case POP:
            {
                ip++;
                proc_elem_t dest = proc.code[++ip];
                //fprintf(stderr, "dest = %d\n", dest);
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
                proc_elem_t addr = proc.code[++ip];
                ip = (size_t)addr;
                //abort();
                break;
            }
            case JA:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b > a)
                    ip = (size_t)addr;
                break;
            }
            case JAE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b >= a)
                    ip = (size_t)addr;
                break;
            }
            case JB:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b < a)
                    ip = (size_t)addr;
                break;
            }
            case JBE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b <= a)
                    ip = (size_t)addr;
                break;
            }
            case JE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b == a)
                    ip = (size_t)addr;
                break;
            }
            case JME:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
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
    free(proc.code);
    stack_destroy(proc.stk);
}

static void dump(FILE* fp, const struct spu* proc, size_t ip)
{
    fprintf(fp, "------------------------------------\n");
    fprintf(fp, "code ");
    for (size_t i = 0; i < proc->code_size; i++)
        fprintf(fp, "%2X ", i);
    fprintf(fp, "\n     ");
    for (size_t i = 0; i < proc->code_size; i++)
    {
        fprintf(fp, "%2X ", proc->code[i]);
    }
    fprintf(fp, "\n      ");
    for (size_t i = 0; i < ip; i++)
        fprintf(fp, "~~~");
    fprintf(fp, "^~ip=%zu\n", ip);
    fprintf(fp, "------------------------------------\n\n");
}

static int pr (const void* p)
{
    const proc_elem_t* pi = (const proc_elem_t*)p;
    fprintf(stderr, "%ld", *pi);
    return 0;
}

