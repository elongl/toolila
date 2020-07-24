#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"

#define REDIRECT_STDERR " 2>&1"

void run_shell_cmd(char *cmd, char *output)
{
    printf("Running a shell command: %s\n", cmd);
    char full_cmd[strlen(cmd) + strlen(REDIRECT_STDERR) + 1];
    sprintf(full_cmd, "%s %s", cmd, REDIRECT_STDERR);
    FILE *fp = popen(full_cmd, "r");
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

void shell(unsigned int peer_fd)
{
    puts("Received SHELL command.");
    char output[SHELL_OUTPUT_BUFSIZE] = {};
    char cmd[SHELL_CMD_BUFSIZE] = {};

    if (read(peer_fd, cmd, sizeof(cmd)) < 0)
    {
        perror("Failed to read the shell command from peer.");
        exit(EXIT_FAILURE);
    }
    run_shell_cmd(cmd, output);
    puts("Sending the command output to the peer.");
    if (write(peer_fd, output, sizeof(output)) < 0)
    {
        perror("Failed to send output to the peer.");
        exit(EXIT_FAILURE);
    }
}