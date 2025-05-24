#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#define DEVICE "/dev/uio0"
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

    printf("[ExampleApp] Starting message sender loop...\n");

    int counter = 0;
    while (1) {
        if (buf->ready == 0) {
            snprintf(buf->data, sizeof(buf->data), "Message %d from ExampleApp", counter++);
            buf->ready = 1;
            printf("[ExampleApp] Wrote: %s\n", buf->data);
        }
        usleep(500000);  // 0.5s delay
    }

    munmap(buf, SIZE);
    close(fd);
    return 0;
}
