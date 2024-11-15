#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <math.h>
#include <assert.h>
#include "stack/stack.h"
#include "cmds.h"

struct spu {
    struct stack_t* stk;
    int ip;
    proc_elem_t* code;
    size_t code_size;
    proc_elem_t reg[4];
    proc_elem_t* ram;
    size_t ram_size;
    struct stack_t* funcs_stk;
};

static int pr (const void* p);
static void dump(FILE* fp, const struct spu* proc, size_t ip);
static void push_args(struct spu* proc, size_t* ip);
static void pop_args(struct spu* proc, size_t* ip);
static proc_elem_t get_arg_push(proc_elem_t type,
                           proc_elem_t arg1,
                           proc_elem_t arg2);
static proc_elem_t get_arg_pop(struct spu* proc,
                               proc_elem_t type,
                               proc_elem_t arg1,
                               proc_elem_t arg2);

int main()
{
    proc_elem_t cmd = 0;
    size_t ip = 0;
    struct spu proc= {};
    struct stat filedata = {};
    FILE* temp_p = fopen("log.txt", "w");
    if (temp_p == NULL)
    {
        fprintf(stderr, "FAILED TO OPEN LOG FILE\n");
        assert(0);
    }
    stderr = temp_p;

    FILE* fp = fopen("cmds.bin", "rb");
    stat("cmds.bin", &filedata);
    proc.code = (proc_elem_t*)calloc(sizeof(proc_elem_t),
                                     (size_t)filedata.st_size);
    proc.code_size = (size_t)filedata.st_size/sizeof(proc_elem_t);
    proc.ram_size = 1024;
    proc.ram = (proc_elem_t*)calloc(sizeof(proc_elem_t), proc.ram_size);
    fread(proc.code, 1, (size_t)filedata.st_size, fp);
    proc.stk = stack_init(sizeof(proc_elem_t), 4);
    proc.funcs_stk = stack_init(sizeof(proc_elem_t), 4);

    for(size_t i = 0; i < proc.code_size; i++)
    {
        fprintf(stderr, "%3zu ", i);
    }
    fprintf(stderr, "\n");
    for(size_t i = 0; i < proc.code_size; i++)
    {
        fprintf(stderr, "%3ld ", proc.code[i]);
    }
    fputc('\n', stderr);

    for(ip = 0; ip < proc.code_size; ip++)
    {
        proc_elem_t a, b;
        cmd = proc.code[ip];
        //fprintf(stderr, "%d\n", cmd);
        //stack_printf(proc.stk, pr);
        //dump(stderr, &proc, ip);
        switch (cmd)
        {
            case PUSH:
                push_args(&proc, &ip);
                break;
            case POP:
                pop_args(&proc, &ip);
                break;
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
                a = b*a/PRECISION;
                stack_push(proc.stk, &a);
                break;
            case DIV:
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                a = (b*PRECISION)/a;
                stack_push(proc.stk, &a);
                break;
            case SQRT:
                stack_pop(proc.stk, &a);
                a = (proc_elem_t)sqrt(a*PRECISION);
                //fprintf(stderr, "a = %ld\n", a);
                stack_push(proc.stk, &a);
                break;
            case OUT:
                stack_pop(proc.stk, &a);
                fprintf(stderr, "%.4f\n", (float)(a)/PRECISION);
                break;
            case HLT:
                goto stop_reading;
                break;
            case LBL:
                break;
            case JMP:
            {
                proc_elem_t addr = proc.code[++ip];
                ip = (size_t)addr - 1;
                break;
            }
            case JA:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b > a)
                    ip = (size_t)addr - 1;
                break;
            }
            case JAE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b >= a)
                {
                    ip = (size_t)addr - 1;
                    //fprintf(stderr, "JUMPING\n");
                }
                break;
            }
            case JB:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b < a)
                    ip = (size_t)addr - 1;
                break;
            }
            case JBE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b <= a)
                    ip = (size_t)addr - 1;
                break;
            }
            case JE:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b == a)
                    ip = (size_t)addr - 1;
                break;
            }
            case JME:
            {
                stack_pop(proc.stk, &a);
                stack_pop(proc.stk, &b);
                proc_elem_t addr = proc.code[++ip];
                if (b != a)
                    ip = (size_t)addr - 1;
                break;
            }
            case DRAW:
            {
                for (int y = 0; y < 32; y++)
                {
                    for (int x = 0; x < 32; x++)
                    {
                        fputc((int)proc.ram[32*y + x]/PRECISION, stdout);
                        fputc(' ', stdout);
                    }
                    fputc('\n', stdout);
                }
                break;
            }
            case CALL:
            {
                proc_elem_t return_addr = ip + 2;
                proc_elem_t addr = proc.code[++ip];
                stack_push(proc.funcs_stk, &return_addr);
                ip = (size_t)addr - 1;
                break;
            }
            case RET:
            {
                proc_elem_t addr = 0;
                stack_pop(proc.funcs_stk, &addr);
                ip = (size_t)addr - 1;
                break;
            }
            case IN:
            {
                float f;
                scanf("%f", &f);
                a = (proc_elem_t)(f*PRECISION);
                stack_push(proc.stk, &a);
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
    free(proc.ram);
    stack_destroy(proc.stk);
    stack_destroy(proc.funcs_stk);
}

static void push_args(struct spu* proc, size_t* ip)
{
    proc_elem_t arg1 = 0, arg2 = 0, arg = 0;
    proc_elem_t type = proc->code[++(*ip)];
    if (type & mask_numb)
        arg1 = proc->code[++(*ip)];
    if (type & mask_reg)
        arg2 = proc->reg[proc->code[++(*ip)]];

    arg = get_arg_push(type, arg1, arg2);

    if (type & mask_mem)
    {
        arg /= PRECISION;
        stack_push(proc->stk, &proc->ram[arg]);
    }
    else
    {
        stack_push(proc->stk, &arg);
    }
}

static void pop_args(struct spu* proc, size_t* ip)
{
    proc_elem_t arg1 = 0, arg2 = 0, arg = 0;
    proc_elem_t type = proc->code[++(*ip)];
    if (type & mask_numb && !type & mask_reg && !type & mask_mem)
    {
        fprintf(stderr, "ATTEMPT TO POP TO A NUMBER\n");
        assert(0);
    }
    if (type & mask_numb)
        arg1 = proc->code[++(*ip)];
    if (type & mask_reg)
        arg2 = proc->code[++(*ip)];

    fprintf(stdout, "type = %d, arg1 = %d, arg2 = %d\n", type, arg1, arg2);
    arg = get_arg_pop(proc, type, arg1, arg2);
    fprintf(stdout, "arg = %d\n", arg);

    if (type & mask_mem)
        stack_pop(proc->stk, &proc->ram[arg]);
    else
        stack_pop(proc->stk, &proc->reg[arg]);
}

static proc_elem_t get_arg_push(proc_elem_t type,
                           proc_elem_t arg1,
                           proc_elem_t arg2)
{
    proc_elem_t arg = 0;
    if (type & mask_numb && type & mask_reg)
    {
        if (type & mask_plus)
            arg = arg1 + arg2;
        if (type & mask_star)
            arg = arg1 * arg2 / PRECISION;
        if (type & mask_minus)
            arg = arg1 - arg2;
        if (type & mask_backminus)
            arg = arg2 - arg1;
        if (type & mask_slash)
            arg = (arg1 * PRECISION) / arg2;
        if (type & mask_backslash)
            arg = (arg2 * PRECISION) / arg1;
    }
    else
    {
        if (type & mask_numb)
        {
            arg = arg1;
            return arg;
        }
        if (type & mask_reg)
        {
            arg = arg2;
            return arg;
        }
        fprintf(stderr, "WRONG ARG %ld: NOTHING TO PUSH\n", arg);
        assert(0);
    }
    return arg;
}

static proc_elem_t get_arg_pop(struct spu* proc,
                               proc_elem_t type,
                               proc_elem_t arg1,
                               proc_elem_t arg2)
{
    proc_elem_t arg = 0;
    if (type & mask_numb && type & mask_reg)
    {
        if (type & mask_mem)
        {
            if (type & mask_plus)
                arg = arg1 + proc->reg[arg2]/PRECISION;
            if (type & mask_star)
                arg = arg1 * proc->reg[arg2]/PRECISION;
            if (type & mask_minus)
                arg = arg1 - proc->reg[arg2]/PRECISION;
            if (type & mask_backminus)
                arg = proc->reg[arg2]/PRECISION - arg1;
            if (type & mask_slash)
                arg = arg1 / proc->reg[arg2]/PRECISION;
            if (type & mask_backslash)
                arg = proc->reg[arg2]/PRECISION / arg1;
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        if (type & mask_numb && type & mask_mem)
        {
            arg = arg1;
            return arg;
        }
        if (type & mask_reg && type & mask_mem)
        {
            arg = proc->reg[arg2] / PRECISION;
            return arg;
        }
        if (type & mask_reg)
        {
            arg = arg2;
            return arg;
        }
        fprintf(stderr, "WRONG ARG %ld: NOTHING TO POP\n", arg);
        assert(0);
    }
    return arg;
}

static void dump(FILE* fp, const struct spu* proc, size_t ip)
{
    fprintf(fp, "------------------------------------\n");
    fprintf(fp, "code ");
    for (size_t i = 0; i < proc->code_size; i++)
        fprintf(fp, "%2X ", (unsigned int)i);
    fprintf(fp, "\n     ");
    for (size_t i = 0; i < proc->code_size; i++)
    {
        if (proc->code[i] >= 0)
            fprintf(fp, "%2X ", (unsigned int)proc->code[i]);
        else
            fprintf(fp, " XX");
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

