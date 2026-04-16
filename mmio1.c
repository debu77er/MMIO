#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define the physical address of your hardware's MMIO region
// Replace with your device's specific address
#define MMIO_BASE_PHYS_ADDR 0x40000000
#define MMIO_REGION_SIZE    0x1000  // Size of the region to map

// Offset of the register controlling sound output
#define SOUND_REG_OFFSET    0x00

// Duration of sound in milliseconds
#define SOUND_DURATION_MS  1000

// Function to get current time in milliseconds
uint64_t get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec) * 1000 + (ts.tv_nsec / 1000000);
}

int main() {
    int fd;
    void *mapped_base;
    volatile uint32_t *sound_reg;

    // Open /dev/mem with read/write permissions
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("Failed to open /dev/mem");
        return EXIT_FAILURE;
    }

    // Map the physical address into user space
    mapped_base = mmap(NULL, MMIO_REGION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MMIO_BASE_PHYS_ADDR);
    if (mapped_base == MAP_FAILED) {
        perror("Failed to mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    // Calculate the address of the sound register
    sound_reg = (volatile uint32_t *)((char *)mapped_base + SOUND_REG_OFFSET);

    printf("Starting sound...\n");

    uint64_t start_time = get_time_ms();

    // Generate a square wave for SOUND_DURATION_MS milliseconds
    while (get_time_ms() - start_time < SOUND_DURATION_MS) {
        // Turn sound on
        *sound_reg = 1;
        usleep(1000); // 1 ms delay

        // Turn sound off
        *sound_reg = 0;
        usleep(1000); // 1 ms delay
    }

    printf("Sound finished.\n");

    // Cleanup
    munmap(mapped_base, MMIO_REGION_SIZE);
    close(fd);

    return EXIT_SUCCESS;
}
