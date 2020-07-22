#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

void shell(char *cmd, char *output)
{

    FILE *fp;
    char path[SHELL_BUFSIZE];

    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while (fread(output, sizeof(path), 1, fp) != 0)
    {
        printf("%s", path);
    }

    pclose(fp);
}
