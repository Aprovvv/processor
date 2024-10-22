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
        {
            fprintf(output, "%d\n", HLT);
            continue;
        }
        if (strcmp(cmd, "push") == 0)
        {
            fprintf(output, "%d\n", PUSH);
            fscanf(input, "%ld", &arg);
            fprintf(output, "%ld\n", arg);
            fscanf(input, "%ld", &arg);
            fprintf(output, "%ld\n", arg);
            continue;
        }
        if (strcmp(cmd, "pop") == 0)
        {
            fprintf(output, "%d\n", POP);
            fscanf(input, "%ld", &arg);
            fprintf(output, "%ld\n", arg);
            continue;
        }
        if (strcmp(cmd, "out") == 0)
        {
            fprintf(output, "%d\n", OUT);
            continue;
        }
        if (strcmp(cmd, "sum") == 0)
        {
            fprintf(output, "%d\n", SUM);
            continue;
        }
        if (strcmp(cmd, "sub") == 0)
        {
            fprintf(output, "%d\n", SUB);
            continue;
        }
        if (strcmp(cmd, "mult") == 0)
        {
            fprintf(output, "%d\n", MULT);
            continue;
        }
        if (strcmp(cmd, "div") == 0)
        {
            fprintf(output, "%d\n", DIV);
            continue;
        }
        if (strcmp(cmd, "jmp") == 0)
        {
            fprintf(output, "%d ", JMP);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "ja") == 0)
        {
            fprintf(output, "%d\n", JA);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "jae") == 0)
        {
            fprintf(output, "%d\n", JAE);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "jb") == 0)
        {
            fprintf(output, "%d ", JB);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "jbe") == 0)
        {
            fprintf(output, "%d\n", JBE);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "je") == 0)
        {
            fprintf(output, "%d\n", JE);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "jme") == 0)
        {
            fprintf(output, "%d\n", JME);
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%s\n", lbl_name);
            continue;
        }
        if (strcmp(cmd, "lbl") == 0)
        {
            char lbl_name[LBL_SIZE] = "";
            get_str(input, lbl_name, LBL_SIZE);
            fprintf(output, "%d\n%s\n", LBL, lbl_name);
            continue;
        }
        fprintf(stderr, "SNTXERR: %s\n", cmd);
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
