
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_MEM_SIZE 4096

int main() {
    int fd = open("/dev/uio0", O_RDWR);
    if (fd < 0) {
        perror("open /dev/uio0");
        return 1;
    }

    char *shared = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("ExampleApp writing message to shared memory...\n");
    snprintf(shared, SHARED_MEM_SIZE, "Hello from ExampleApp (UIO0)");

    munmap(shared, SHARED_MEM_SIZE);
    close(fd);
    return 0;
}
