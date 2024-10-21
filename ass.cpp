#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmds.h"

int main()
{
    FILE* input = fopen("commands.txt", "r");
    FILE* output = fopen("cmds.txt", "w");
    char cmd[30];
    long int code = 0;
    long int arg = 0;
    int put_arg = 0;
    while(fscanf(input, "%s", cmd) != EOF)
    {
        printf("%s ", cmd);
        if (strcmp(cmd, "hlt") == 0)
            code = hlt;
        else if (strcmp(cmd, "push") == 0)
        {
            code = push;
            fscanf(input, "%ld", &arg);
            put_arg = 1;
        }
        else if (strcmp(cmd, "out") == 0)
            code = out;
        else if (strcmp(cmd, "sum") == 0)
            code = sum;
        else if (strcmp(cmd, "sub") == 0)
            code = sub;
        else if (strcmp(cmd, "mult") == 0)
            code = mult;
        else if (strcmp(cmd, "div") == 0)
            code = divv;
        printf("%ld\n", code);
        fprintf(output, "%ld\n", code);
        if (put_arg)
        {
            fprintf(output, "%ld\n", arg);
            put_arg = 0;
        }
    }
    fclose(input);
    fclose(output);
}

