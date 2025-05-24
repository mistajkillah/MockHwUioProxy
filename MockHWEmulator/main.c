
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_MEM_SIZE 4096

int main() {
    int fd = open("/dev/uio1", O_RDONLY);
    if (fd < 0) {
        perror("open /dev/uio1");
        return 1;
    }

    char *shared = mmap(NULL, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("MockHWEmulator read from shared memory: %s\n", shared);

    munmap(shared, SHARED_MEM_SIZE);
    close(fd);
    return 0;
}
