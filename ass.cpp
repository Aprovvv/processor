#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cmds.h"

static void get_str(FILE* fp, char* str, int size);

int main()
{
    FILE* input = fopen("commands.txt", "r");
    FILE* output = fopen("cmds.txt", "w");
    char cmd[30];
    long int arg = 0;
    while(fscanf(input, "%s", cmd) != EOF)
    {
        printf("%s ", cmd);
        if (strcmp(cmd, "hlt") == 0)
            fprintf(output, "%d\n", hlt);
        else if (strcmp(cmd, "push") == 0)
        {
            fprintf(output, "%d\n", push);
            fscanf(input, "%ld", &arg);
            fprintf(output, "%ld\n", arg);
        }
        else if (strcmp(cmd, "out") == 0)
            fprintf(output, "%d\n", out);
        else if (strcmp(cmd, "sum") == 0)
            fprintf(output, "%d\n", sum);
        else if (strcmp(cmd, "sub") == 0)
            fprintf(output, "%d\n", sub);
        else if (strcmp(cmd, "mult") == 0)
            fprintf(output, "%d\n", divv);
        else if (strcmp(cmd, "div") == 0)
            fprintf(output, "%d\n", mult);
        else if (strcmp(cmd, "jmp") == 0)
        {
            fprintf(output, "%d ", jmp);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        else if (strcmp(cmd, "ja") == 0)
            fprintf(output, "%d\n", ja);
        else if (strcmp(cmd, "jae") == 0)
            fprintf(output, "%d\n", jae);
        else if (strcmp(cmd, "jb") == 0)
            fprintf(output, "%d\n", jb);
        else if (strcmp(cmd, "jbe") == 0)
            fprintf(output, "%d\n", jbe);
        else if (strcmp(cmd, "je") == 0)
            fprintf(output, "%d\n", je);
        else if (strcmp(cmd, "jme") == 0)
            fprintf(output, "%d\n", jme);
        else if (strcmp(cmd, "lbl") == 0)
        {
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%d\n%s\n", lbl, lbl_name);
            continue;
        }
        //printf("%ld\n", code);
    }
    fclose(input);
    fclose(output);
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
