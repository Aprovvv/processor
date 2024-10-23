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
            get_str(input, l.name, LBL_SIZE);
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
    get_str(input, arg, CMD_SIZE);
    printf("!!! arg = %s\n", arg);
    if (strchr(arg, '[') == NULL)
    {
        (*ip)+=2;
        int sign  = sscanf(arg, "%ld + %s", &arg1, arg2);
        if (sign == 2)
        {
            type = mask_numb + mask_reg + mask_plus;
            stack_push(stk, &type);
            stack_push(stk, &arg1);
            arg2_num = reg_name(arg2);
            stack_push(stk, &arg2_num);
            ip++;
            return;
        }
        sign = sscanf(arg, "%ld - %s", &arg1, arg2);
        if (sign == 2)
        {
            type = mask_numb + mask_reg + mask_minus;
            stack_push(stk, &type);
            stack_push(stk, &arg1);
            arg2_num = reg_name(arg2);
            stack_push(stk, &arg2_num);
            ip++;
            return;
        }
        sign = sscanf(arg, "%ld * %s", &arg1, arg2);
        if (sign == 2)
        {
            type = mask_numb + mask_reg + mask_star;
            stack_push(stk, &type);
            stack_push(stk, &arg1);
            arg2_num = reg_name(arg2);
            stack_push(stk, &arg2_num);
            ip++;
            return;
        }
        sign = sscanf(arg, "%ld / %s", &arg1, arg2);
        if (sign == 2)
        {
            type = mask_numb + mask_reg + mask_slash;
            stack_push(stk, &type);
            stack_push(stk, &arg1);
            arg2_num = reg_name(arg2);
            stack_push(stk, &arg2_num);
            ip++;
            return;
        }
        if (sign == 1)
        {
            type = mask_numb;
            stack_push(stk, &type);
            stack_push(stk, &arg1);
            return;
        }
        if (sscanf(arg, "%s", arg2))
        {
            if ((arg2_num = reg_name(arg2)) >= 0)
            {
                printf("type = %ld; arg2_num = %ld; arg2 = %s; ax = %d\n", type, arg2_num, arg2, ax);
                type = mask_reg;
                stack_push(stk, &type);
                stack_push(stk, &arg2_num);
                return;
            }
        }
        fprintf(stderr, "SNTXERR: undefined argument (%s)\n", arg);
        abort();
    }
}

static void pop_args(struct stack_t* stk, FILE* input, size_t* ip)
{
    char arg[CMD_SIZE] = "";
    proc_elem_t a = 0;
    get_str(input, arg, CMD_SIZE);
    printf("!!! arg = %s\n", arg);
    if (strchr(arg, '[') == NULL)
    {
        (*ip)+=2;
        if(strcmp(arg, "ax") == 0)
        {
            a = mask_reg;
            stack_push(stk, &a);
            a = ax;
            stack_push(stk, &a);
            return;
        }
        if(strcmp(arg, "bx") == 0)
        {
            a = mask_reg;
            stack_push(stk, &a);
            a = bx;
            stack_push(stk, &a);
            return;
        }
        if(strcmp(arg, "cx") == 0)
        {
            a = mask_reg;
            stack_push(stk, &a);
            a = cx;
            stack_push(stk, &a);
            return;
        }
        if(strcmp(arg, "dx") == 0)
        {
            a = mask_reg;
            stack_push(stk, &a);
            a = dx;
            stack_push(stk, &a);
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
    get_str(fp, name, LBL_SIZE);
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
        if (ch == 0 || isspace(ch))
            break;
        str[i] = ch;
        i++;
    }
    str[i] = 0;
}
