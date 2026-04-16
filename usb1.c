#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

// Path to sysfs usb devices
#define SYS_USB_PATH "/sys/bus/usb/devices"

// Helper function to check if a directory entry is a device directory
int is_usb_device(const struct dirent *entry) {
    // USB device directories typically have numeric names or contain "usb" in their name
    // For simplicity, check for directories that are not '.' or '..'
    if (entry->d_name[0] == '.') {
        return 0;
    }
    return 1;
}

// Function to read a file content into a buffer
int read_sysfs_file(const char *path, char *buffer, size_t size) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    if (fgets(buffer, size, fp) == NULL) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    // Remove trailing newline if present
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    return 0;
}

int main() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(SYS_USB_PATH);
    if (!dir) {
        perror("Failed to open sysfs usb devices directory");
        return 1;
    }

    printf("Connected USB Devices:\n");
    printf("%-20s %-20s %-20s", "Device", "ID Vendor", "ID Product");

    while ((entry = readdir(dir)) != NULL) {
        if (!is_usb_device(entry))
            continue;

        // Construct path to device directory
        char device_path[512];
        snprintf(device_path, sizeof(device_path), "%s/%s", SYS_USB_PATH, entry->d_name);

        // Read vendor ID and product ID
        char idVendor[16] = {0};
        char idProduct[16] = {0};
        char path_vendor[512], path_product[512];

        snprintf(path_vendor, sizeof(path_vendor), "%s/idVendor", device_path);
        snprintf(path_product, sizeof(path_product), "%s/idProduct", device_path);

        if (read_sysfs_file(path_vendor, idVendor, sizeof(idVendor)) != 0)
            continue;
        if (read_sysfs_file(path_product, idProduct, sizeof(idProduct)) != 0)
            continue;

        printf("%-20s %-20s %-20s", entry->d_name, idVendor, idProduct);
    }

    closedir(dir);
    return 0;
}
