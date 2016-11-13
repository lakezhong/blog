#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kallsyms.h>


static int TestKallsymsInit(void)
{
    // the one with EXPORT_SYMBOL
    unsigned long vfs_read_address = kallsyms_lookup_name("vfs_read");

    // the one without EXPORT_SYMBOL, fsnotify_alloc_group needs kernel version >= 2.6.36
    unsigned long fsnotify_alloc_group_address = kallsyms_lookup_name("fsnotify_alloc_group");

    printk("vfs_read_address = 0x%lx\n", vfs_read_address);
    printk("fsnotify_alloc_group_address = 0x%lx\n", fsnotify_alloc_group_address);

    printk("init module success\n");
    return 0;
}

static void TestKallsymsExit(void)
{
    printk("exit module\n");
}

module_init(TestKallsymsInit);
module_exit(TestKallsymsExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lakezhong");
