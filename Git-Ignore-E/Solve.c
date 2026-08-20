#include <fcntl.h>

int create(const char *path, mode_t mode)
{
    return creat(path, mode);
}

