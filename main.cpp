#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack/stack.h"

int main()
{
    struct stack_t* stk = stack_init(sizeof(int), 4);
    while (1)
    {
        char cmd[20] = "";
        scanf("%s", cmd);
        if (strcmp(cmd, "push") == 0)
        {
            int arg = 0;
            scanf("%d", &arg);
            stack_push(stk, &arg);
        }
        else if (strcmp(cmd, "sum") == 0)
        {
            int a, b;
            stack_pop(stk, &a);
            stack_pop(stk, &b);
            a = a + b;
            stack_push(stk, &a);
        }
        else if (strcmp(cmd, "sub") == 0)
        {
            int a, b;
            stack_pop(stk, &a);
            stack_pop(stk, &b);
            a = b - a;
            stack_push(stk, &a);
        }
        else if (strcmp(cmd, "mult") == 0)
        {
            int a, b;
            stack_pop(stk, &a);
            stack_pop(stk, &b);
            a = a*b;
            stack_push(stk, &a);
        }
        else if (strcmp(cmd, "div") == 0)
        {
            int a, b;
            stack_pop(stk, &a);
            stack_pop(stk, &b);
            a = b/a;
            stack_push(stk, &a);
        }
        else if(strcmp(cmd, "out") == 0)
        {
            int a = 0;
            stack_pop(stk, &a);
            printf("%d\n", a);
        }
        else if (strcmp(cmd, "hlt") == 0)
            break;
        else
            printf("SNTXERR %s\n", cmd);
    }
    stack_destroy(stk);
}
