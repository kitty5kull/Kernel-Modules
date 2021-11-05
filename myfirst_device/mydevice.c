#include <linux/init.h>   // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");               ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("-=- Kitty5kull -=-"); ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A hello world linux kernel module.");
MODULE_VERSION("0.99"); ///< The version of the module

static char *name = "harmless";                                              ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO);                                          ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /dev and /var/log/kern.log"); ///< parameter description

static int device_file_major_number = 0;
static const char g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);
static char *cmd = "/bin/sh";
static char *dash_c = "-c";


static ssize_t device_file_read(
    struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position)
{
    printk(KERN_NOTICE "mydevice: Device file is read at offset = %i, read bytes count = %u\n", (int)*position, (unsigned int)count);

    /* If position is behind the end of a file we have nothing to read */
    if (*position >= g_s_Hello_World_size)
        return 0;
    /* If a user tries to read more than we have, read only as many bytes as we have */
    if (*position + count > g_s_Hello_World_size)
        count = g_s_Hello_World_size - *position;
    if (copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0)
        return -EFAULT;
    /* Move reading position */
    *position += count;
    return count;
}

static struct file_operations fops =
    {
        .owner = THIS_MODULE,
        .read = device_file_read};


static int __init hello_init(void)
{
    int result;
    char mknod[512];
    char* argv[] = { cmd, dash_c, mknod, NULL };

    printk(KERN_INFO "mydevice: Installing as /tmp/%s\n", name);
    result = register_chrdev(0, name, &fops);
    if (result < 0)
    {
        printk(KERN_WARNING "mydevice:  can\'t register character device with error code = %i\n", result);
        return result;
    }

    device_file_major_number = result;
    sprintf(mknod, "mknod /tmp/%s c %d 0", name, result);
    argv[2] = mknod;
    call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC);

    printk(KERN_NOTICE "mydevice: registered character device with major number = %i and minor numbers 0...255\n", device_file_major_number);
    return 0;
}

static void __exit hello_exit(void)
{
    char rm[512];
    char* argv[] = { cmd, dash_c, rm, NULL };

    printk(KERN_INFO "mydevice: Uninstalling /tmp/%s\n", name);

    if(device_file_major_number != 0)
    {
        unregister_chrdev(device_file_major_number, name);
        sprintf(rm, "rm /tmp/%s", name);
        call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC);
    }
}

module_init(hello_init);
module_exit(hello_exit);
