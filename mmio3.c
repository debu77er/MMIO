#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/init.h>
#include <linux/ioport.h>

int mmiowr_init(void)
{
    volatile u32 *addr = NULL;
    int ret = 0;

    printk(KERN_INFO "Hello World!\n");
    addr = ioremap_wc(0x11003000, 0x40);
    if (!addr) {
        printk("Error: Failed to remap 0x11003000\n");
    } else {
        printk("XXXXXX Data_0: 0x%x, Data_1: 0x%d\n", addr[0], addr[1]);
    }
    iounmap(addr);
    return ret;
}

void mmiowr_exit(void)
{
    printk(KERN_INFO "Bye World!\n");
}

module_init(mmiowr_init);
module_exit(mmiowr_exit);
MODULE_LICENSE("GPL");