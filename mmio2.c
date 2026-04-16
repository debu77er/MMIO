#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>

// Replace these with your hardware specifics
#define MMIO_BASE_PHYS_ADDR 0x40000000  // Example physical address
#define MMIO_REGION_SIZE   0x1000      // Size of the MMIO region (4KB)

volatile uint32_t *mmio_base = NULL;

// Function to initialize MMIO mapping
int init_mmio(const char *device) {
    int fd = open(device, O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("Failed to open /dev/mem");
        return -1;
    }

    // Map the MMIO region into user space
    mmio_base = mmap(NULL, MMIO_REGION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MMIO_BASE_PHYS_ADDR);
    if (mmio_base == MAP_FAILED) {
        perror("Failed to mmap");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Function to cleanup MMIO
void cleanup_mmio() {
    if (mmio_base && mmio_base != MAP_FAILED) {
        munmap((void*)mmio_base, MMIO_REGION_SIZE);
    }
}

// Function to "play" a note by writing to MMIO
void play_note(uint8_t note) {
    // For demonstration, assume the first register controls the note
    // Write note value to the register
    mmio_base[0] = note;
    printf("Playing note: %d", note);
    // Add delay if needed
    usleep(500000); // 0.5 seconds
    // Stop note
    mmio_base[0] = 0;
}

// Main program
int main() {
    const char *device = "/dev/mem";

    if (init_mmio(device) != 0) {
        fprintf(stderr, "Failed to initialize MMIO");
        return EXIT_FAILURE;
    }

    printf("MMIO Piano App started.\n");
    printf("Press numbers 1-7 to play notes, 0 to exit.\n");

    int ch;
    while (1) {
        printf("Enter note (1-7), 0 to quit: ");
        ch = getchar();
        while (getchar() != '\n'); // flush input buffer

        if (ch == '0') {
            break;
        } else if (ch >= '1' && ch <= '7') {
            uint8_t note = ch - '0'; // simple mapping
            play_note(note);
        } else {
            printf("Invalid input.\n");
        }
    }

    cleanup_mmio();
    printf("Exiting.\n");
    return 0;
}
