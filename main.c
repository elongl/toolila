#include <stdio.h>
#include "shell.h"

int main()
{
    char out[SHELL_BUFSIZE];
    shell("ls -lah", out);
    printf("%s", out);
}