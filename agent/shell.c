#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

#define CMD

void shell(char *cmd, char *output)
{
    printf("Running a shell command: %s\n", cmd);
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        printf("Failed to run command.");
        exit(EXIT_FAILURE);
    }
    if (fread(output, SHELL_OUTPUT_BUFSIZE, 1, fp) < 0)
    {
        perror("Failed to read from socket.");
        exit(EXIT_FAILURE);
    }
    pclose(fp);
}
