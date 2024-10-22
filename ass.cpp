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

static void get_str(FILE* fp, char* str, int size);
static long search_lbl(FILE* fp, struct stack_t* lbl_stack);

int main()
{
    struct stack_t* stk = stack_init(sizeof(long), 4);
    struct stack_t* lbl_stack = stack_init(sizeof(struct lbl_t), 4);
    FILE* input = fopen("commands.txt", "r");
    FILE* output = fopen("cmds.bin", "wb");
    char cmd[30];
    long arg = 0;
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
            fscanf(input, "%ld", &arg);
            stack_push(stk, &arg);
            fscanf(input, "%ld", &arg);
            stack_push(stk, &arg);
            ip += 2;
            continue;
        }
        if (strcmp(cmd, "pop") == 0)
        {
            arg = POP;
            stack_push(stk, &arg);
            fscanf(input, "%ld", &arg);
            stack_push(stk, &arg);
            ip++;
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
            long addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "ja") == 0)
        {
            arg = JA;
            stack_push(stk, &arg);
            long addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jae") == 0)
        {
            arg = JAE;
            stack_push(stk, &arg);
            long addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jb") == 0)
        {
            arg = JB;
            stack_push(stk, &arg);
            long addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jbe") == 0)
        {
            arg = JBE;
            stack_push(stk, &arg);
            long addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "je") == 0)
        {
            arg = JE;
            stack_push(stk, &arg);
            long addr = search_lbl(input, lbl_stack);
            stack_push(stk, &addr);
            ip++;
            continue;
        }
        if (strcmp(cmd, "jme") == 0)
        {
            arg = JME;
            stack_push(stk, &arg);
            long addr = search_lbl(input, lbl_stack);
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
            l.addr = (long)ip;
            stack_push(lbl_stack, &l);
            fprintf(stderr, "l.name = %s; l.addr = %d\n", l.name, l.addr);
            continue;
        }
        fprintf(stderr, "SNTXERR: %s\n", cmd);
    }
    fwrite(stack_data(stk), sizeof(long), stack_size(stk), output);
    stack_destroy(stk);
    stack_destroy(lbl_stack);
    fclose(input);
    fclose(output);
}

static long search_lbl(FILE* fp, struct stack_t* lbl_stack)
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
