#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define DEVICE "/dev/uio1"
#define SIZE 4096

struct message_buf {
    volatile int ready;
    char data[SIZE - sizeof(int)];
};

int main() {
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open " DEVICE);
        return 1;
    }

    struct message_buf *buf = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("[MockHWEmulator] Waiting for messages...\n");

    while (1) {
        if (buf->ready == 1) {
            printf("[MockHWEmulator] Received: %s\n", buf->data);
            buf->ready = 0;
        }
        usleep(500000);  // 0.5s delay
    }

    munmap(buf, SIZE);
    close(fd);
    return 0;
}
