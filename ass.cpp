#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack/stack.h"
#include "cmds.h"

struct lbl_t {
    proc_elem_t addr;
    char name[LBL_SIZE];
};

static proc_elem_t search_lbl(FILE* fp, struct stack_t* lbl_stack);
static proc_elem_t reg_name(char arg[CMD_SIZE]);
static proc_elem_t get_type(char arg[CMD_SIZE],
                            proc_elem_t* ptr1,
                            proc_elem_t* ptr2,
                            size_t* ip);

static void get_str(FILE* fp, char* str, int size);
static void push_args(struct stack_t* stk, FILE* input, size_t* ip);
static void pop_args(struct stack_t* stk, FILE* input, size_t* ip);
static void jump(struct stack_t* stk,
                 struct stack_t* lbl_stack,
                 FILE* input,
                 size_t* ip,
                 proc_elem_t arg);

int main()
{
    struct stack_t* stk =
        stack_init(sizeof(proc_elem_t), 4);
    struct stack_t* lbl_stack =
        stack_init(sizeof(struct lbl_t), 4);
    FILE* input = fopen("commands.txt", "r");
    FILE* output = fopen("cmds.bin", "wb");
    char cmd[CMD_SIZE];
    proc_elem_t arg = 0;
    size_t ip = -1;
    while(fscanf(input, "%s", cmd) != EOF)
    {
        ip++;
        char* p = NULL;
        if (strcmp(cmd, "push") == 0)
            ip++;
        if (strcmp(cmd, "pop") == 0)
            ip++;
        if ((p = strchr(cmd, ':')) != NULL)
        {
            struct lbl_t l = {};
            *p = 0;
            strcpy(l.name, cmd);
            l.addr = (proc_elem_t)ip;
            stack_push(lbl_stack, &l);
            fprintf(stderr, "l.name = %s; l.addr = %ld\n",
                    l.name, l.addr);
        }
    }
    rewind(input);
    ip = -1;
    while(fscanf(input, "%s", cmd) != EOF)
    {
        printf("%s\n", cmd);
        ip++;
        if (strcmp(cmd, "hlt") == 0)
        {
            arg = HLT;
            stack_push(stk, &arg);
            continue;
        }
        if (strcmp(cmd, "push") == 0)
        {
            arg = PUSH;
            stack_push(stk, &arg);
            push_args(stk, input, &ip);
            continue;
        }
        if (strcmp(cmd, "pop") == 0)
        {
            arg = POP;
            stack_push(stk, &arg);
            pop_args(stk, input, &ip);
            continue;
        }
        if (strcmp(cmd, "out") == 0)
        {
            arg = OUT;
            stack_push(stk, &arg);
            continue;
        }
        if (strcmp(cmd, "sum") == 0)
        {
            arg = SUM;
            stack_push(stk, &arg);
            continue;
        }
        if (strcmp(cmd, "sub") == 0)
        {
            arg = SUB;
            stack_push(stk, &arg);
            continue;
        }
        if (strcmp(cmd, "mult") == 0)
        {
            arg = MULT;
            stack_push(stk, &arg);
            continue;
        }
        if (strcmp(cmd, "div") == 0)
        {
            arg = DIV;
            stack_push(stk, &arg);
            continue;
        }
        if (strcmp(cmd, "jmp") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JMP);
            continue;
        }
        if (strcmp(cmd, "ja") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JA);
            continue;
        }
        if (strcmp(cmd, "jae") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JAE);
            continue;
        }
        if (strcmp(cmd, "jb") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JB);
            continue;
        }
        if (strcmp(cmd, "jbe") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JBE);
            continue;
        }
        if (strcmp(cmd, "je") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JE);
            continue;
        }
        if (strcmp(cmd, "jme") == 0)
        {
            jump(stk, lbl_stack, input, &ip, JME);
            continue;
        }
        char* p = NULL;
        if ((p = strchr(cmd, ':')) != NULL)
        {
            arg = LBL;
            stack_push(stk, &arg);
            continue;
        }
        fprintf(stderr, "SNTXERR: %s\n", cmd);
    }
    fwrite(stack_data(stk), sizeof(proc_elem_t),
           stack_size(stk), output);
    stack_destroy(stk);
    stack_destroy(lbl_stack);
    fclose(input);
    fclose(output);
}

static void jump(struct stack_t* stk,
                 struct stack_t* lbl_stack,
                 FILE* input,
                 size_t* ip,
                 proc_elem_t arg)
{
    stack_push(stk, &arg);
    proc_elem_t addr = search_lbl(input, lbl_stack);
    stack_push(stk, &addr);
    (*ip)++;
}

static void push_args(struct stack_t* stk, FILE* input, size_t* ip)
{
    proc_elem_t type = 0, arg1 = 0, arg2 = 0;
    char arg[CMD_SIZE] = "";
    get_str(input, arg, CMD_SIZE);

    type = get_type(arg, &arg1, &arg2, ip);

    fprintf(stderr, "type = %ld = %#x\n", type, type);
    if (type & mask_numb && type & mask_reg)
    {
        (*ip)++;
        stack_push(stk, &type);
        stack_push(stk, &arg1);
        stack_push(stk, &arg2);
        return;
    }

    if (type & mask_numb)
    {
        stack_push(stk, &type);
        stack_push(stk, &arg1);
        return;
    }
    if (type & mask_reg)
    {
        stack_push(stk, &type);
        stack_push(stk, &arg2);
        return;
    }
}

static void pop_args(struct stack_t* stk, FILE* input, size_t* ip)
{
    proc_elem_t type = 0, arg1 = 0, arg2 = 0;
    char arg[CMD_SIZE] = "";
    get_str(input, arg, CMD_SIZE);

    type = get_type(arg, &arg1, &arg2, ip);

    fprintf(stderr, "type = %ld = %#x\n", type, type);
    if (type & mask_numb && type & mask_reg)
    {
        (*ip)++;
        stack_push(stk, &type);
        stack_push(stk, &arg1);
        stack_push(stk, &arg2);
        return;
    }

    if (type & mask_reg)
    {
        stack_push(stk, &type);
        stack_push(stk, &arg2);
        return;
    }
    if (type & mask_numb)
    {
        fprintf(stderr, "ATTEMPT TO POP TO A NUMBER\n");
    }
}

static proc_elem_t get_type(char arg[CMD_SIZE],
                            proc_elem_t* ptr1,
                            proc_elem_t* ptr2,
                            size_t* ip)
{
    proc_elem_t type = 0;
    proc_elem_t arg1 = 0;
    char arg2[CMD_SIZE] = "";
    int plus, minus, star, slash;
    int backplus, backminus, backstar, backslash;

    if ((strchr(arg, '[') == NULL) != (strchr(arg, ']') == NULL))
    {
        fprintf(stderr, "ONLY ONE BRACKET FOUND IN %s\n", arg);
        abort();
    }
    if (strchr(arg, '[') != NULL && strchr(arg, ']') != NULL)
    {
        // [a+b*s], [rdx+rax*8]
        plus      = sscanf(arg, "[%ld + %s]", &arg1, arg2);
        minus     = sscanf(arg, "[%ld - %s]", &arg1, arg2);
        star      = sscanf(arg, "[%ld * %s]", &arg1, arg2);
        slash     = sscanf(arg, "[%ld / %s]", &arg1, arg2);
        backplus  = sscanf(arg, "[%s + %ld]", arg2, &arg1);
        backminus = sscanf(arg, "[%s - %ld]", arg2, &arg1);
        backstar  = sscanf(arg, "[%s * %ld]", arg2, &arg1);
        backslash = sscanf(arg, "[%s / %ld]", arg2, &arg1);
        type += mask_mem;
    }
    else
    {
        backplus  = sscanf(arg, "%s + %ld", arg2, &arg1);
        backminus = sscanf(arg, "%s - %ld", arg2, &arg1);
        backstar  = sscanf(arg, "%s * %ld", arg2, &arg1);
        backslash = sscanf(arg, "%s / %ld", arg2, &arg1);
        plus      = sscanf(arg, "%ld + %s", &arg1, arg2);
        minus     = sscanf(arg, "%ld - %s", &arg1, arg2);
        star      = sscanf(arg, "%ld * %s", &arg1, arg2);
        slash     = sscanf(arg, "%ld / %s", &arg1, arg2);
    }
    *ptr1 = arg1;
    *ptr2 = reg_name(arg2);
    (*ip)+=2;
    if (plus == 2 || backplus == 2)
        type += mask_numb + mask_reg + mask_plus;
    if (minus == 2)
        type += mask_numb + mask_reg + mask_minus;
    if (backminus == 2)
        type += mask_numb + mask_reg + mask_backminus;
    if (star == 2 || backstar == 2)
        type += mask_numb + mask_reg + mask_star;
    if (slash == 2)
        type += mask_numb + mask_reg + mask_slash;
    if (backslash == 2)
        type += mask_numb + mask_reg + mask_backslash;

    if (type & mask_numb && type & mask_reg)
        return type;

    if (plus == 1)
    {
        type += mask_numb;
        return type;
    }
    if ((*ptr2) >= 0)
    {
        type += mask_reg;
        return type;
    }
    fprintf(stderr, "SNTXERR: undefined argument (%s)\n", arg);
    abort();
}

static proc_elem_t reg_name(char arg[CMD_SIZE])
{
    if (strcmp(arg, "ax") == 0)
        return ax;
    if (strcmp(arg, "bx") == 0)
        return bx;
    if (strcmp(arg, "cx") == 0)
        return cx;
    if (strcmp(arg, "dx") == 0)
        return dx;
    return -1;
}

static proc_elem_t search_lbl(FILE* fp, struct stack_t* lbl_stack)
{
    char name[LBL_SIZE] = "";
    struct lbl_t l = {};
    fscanf(fp, "%s", name);
    fprintf(stderr, "name = %s\n", name);
    for (size_t i = 0; i < stack_size(lbl_stack); i++)
    {
        stack_view(lbl_stack, i, &l);
        if (strcmp(name, l.name) == 0)
        {
            fprintf(stderr, "addr = %ld\n", l.addr);
            return l.addr;
        }
    }
    fprintf(stderr, "UNDEFINED LABEL %s\n", name);
    abort();
}

static void get_str(FILE* fp, char* str, int size)
{
    int i = 0, ch = 0;
    fgetc(fp);
    while (i < size)
    {
        ch = fgetc(fp);
        if (ch == '\n')
            break;
        str[i] = ch;
        i++;
    }
    str[i] = 0;
}
