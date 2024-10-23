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

static void get_str(FILE* fp, char* str, int size);
static proc_elem_t search_lbl(FILE* fp, struct stack_t* lbl_stack);
static void push_args(struct stack_t* stk, FILE* input, size_t* ip);
static void pop_args(struct stack_t* stk, FILE* input, size_t* ip);
static proc_elem_t reg_name(char arg[CMD_SIZE]);

int main()
{
    struct stack_t* stk = stack_init(sizeof(proc_elem_t), 4);
    struct stack_t* lbl_stack = stack_init(sizeof(struct lbl_t), 4);
    FILE* input = fopen("commands.txt", "r");
    FILE* output = fopen("cmds.bin", "wb");
    char cmd[CMD_SIZE];
    proc_elem_t arg = 0;
    size_t ip = -1;
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
            arg = JMP;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "ja") == 0)
        {
            arg = JA;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jae") == 0)
        {
            arg = JAE;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jb") == 0)
        {
            arg = JB;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jbe") == 0)
        {
            arg = JBE;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "je") == 0)
        {
            arg = JE;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jme") == 0)
        {
            arg = JME;
            stack_push(stk, &arg);
            proc_elem_t addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "lbl") == 0)
        {
            arg = LBL;
            stack_push(stk, &arg);
            struct lbl_t l = {};
            //get_str(input, l.name, LBL_SIZE);
            fscanf(input, "%s", l.name);
            l.addr = (proc_elem_t)ip;
            stack_push(lbl_stack, &l);
            fprintf(stderr, "l.name = %s; l.addr = %ld\n", l.name, l.addr);
            continue;
        }
        fprintf(stderr, "SNTXERR: %s\n", cmd);
    }
    fwrite(stack_data(stk), sizeof(proc_elem_t), stack_size(stk), output);
    stack_destroy(stk);
    stack_destroy(lbl_stack);
    fclose(input);
    fclose(output);
}

static void push_args(struct stack_t* stk, FILE* input, size_t* ip)
{
    char arg[CMD_SIZE] = "";
    proc_elem_t type = 0;
    proc_elem_t arg1 = 0;
    proc_elem_t arg2_num = 0;
    char arg2[CMD_SIZE] = "";
    int plus, minus, star, slash, backplus, backminus, backstar, backslash;
    //get_str(input, arg, CMD_SIZE);
    get_str(input, arg, CMD_SIZE);
    printf("!!! arg = %s\n", arg);
    if ((strchr(arg, '[') == NULL) != (strchr(arg, ']') == NULL))
    {
        fprintf(stderr, "ONLY ONE BRACKET FOUND IN %s\n", arg);
        abort();
    }
    if (strchr(arg, '[') != NULL && strchr(arg, ']') != NULL)
    {
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
    fprintf(stderr, " backplus = %d \n backminus = %d \n backstar = %d \n backslash = %d \n"
            " plus = %d \n minus = %d \n star = %d \n slash = %d \n", backplus, backminus, backstar, backslash, plus, minus, star, slash);
    (*ip)+=2;
    if (plus == 2 || backplus == 2)
        type += mask_numb + mask_reg + mask_plus;
    if (minus == 2 || backminus == 2)
        type += mask_numb + mask_reg + mask_minus;
    if (star == 2 || backstar == 2)
        type += mask_numb + mask_reg + mask_star;
    if (slash == 2 || backslash == 2)
        type += mask_numb + mask_reg + mask_slash;

    fprintf(stderr, "type = %d = %#x\n", type, type);
    if (type & mask_numb && type & mask_reg)
    {
        (*ip)++;
        arg2_num = reg_name(arg2);
        stack_push(stk, &type);
        stack_push(stk, &arg1);
        stack_push(stk, &arg2_num);
        return;
    }

    if (plus == 1)
    {
        type += mask_numb;
        stack_push(stk, &type);
        stack_push(stk, &arg1);
        return;
    }
    if (plus == 0)
    {
        if ((arg2_num = reg_name(arg2)) >= 0)
        {
            //printf("type = %ld; arg2_num = %ld; arg2 = %s; ax = %d\n", type, arg2_num, arg2, ax);
            type += mask_reg;
            stack_push(stk, &type);
            stack_push(stk, &arg2_num);
            return;
        }
    }
    fprintf(stderr, "SNTXERR: undefined argument (%s)\n", arg);
    abort();
}

static void pop_args(struct stack_t* stk, FILE* input, size_t* ip)
{
    char arg[CMD_SIZE] = "";
    proc_elem_t arg_num = 0, type = 0;
    //get_str(input, arg, CMD_SIZE);
    fscanf(input, "%s", arg);
    printf("!!! arg = %s\n", arg);
    if (strchr(arg, '[') == NULL)
    {
        (*ip)+=2;
        if ((arg_num = reg_name(arg)) >= 0)
        {
            type = mask_reg;
            stack_push(stk, &type);
            stack_push(stk, &arg_num);
            return;
        }
        fprintf(stderr, "SNTXERR: undefined argument %s\n", arg);
        abort();
    }
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
    fprintf(stderr, "SNTXERR: undefined argument (%s)\n", arg);
    abort();
}

static proc_elem_t search_lbl(FILE* fp, struct stack_t* lbl_stack)
{
    char name[LBL_SIZE] = "";
    struct lbl_t l = {};
    //get_str(fp, name, LBL_SIZE);
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
        //putchar(ch);
        if (ch == '\n')
            break;
        str[i] = ch;
        i++;
    }
    str[i] = 0;
}
