#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>

#define PROC_NAME "jiffies"

static int jiffies_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "%lu\n", jiffies);
    return 0;
}

static int jiffies_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, jiffies_proc_show, NULL);
}

static const struct proc_ops jiffies_proc_ops = {
    .proc_open    = jiffies_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init jiffies_module_init(void) {
    proc_create(PROC_NAME, 0, NULL, &jiffies_proc_ops);
    return 0;
}

static void __exit jiffies_module_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "jiffies module unloaded\n");
}

module_init(jiffies_module_init);
module_exit(jiffies_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A kernel module that creates /proc/jiffies");
