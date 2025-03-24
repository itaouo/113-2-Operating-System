#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/init.h>

#define PROC_NAME "seconds"

static unsigned long start_jiffies;

static int proc_show(struct seq_file *m, void *v) {
    unsigned long elapsed_jiffies = jiffies - start_jiffies;
    unsigned long elapsed_seconds = elapsed_jiffies / HZ;
    seq_printf(m, "%lu\n", elapsed_seconds);
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init seconds_init(void) {
    start_jiffies = jiffies;
    proc_create(PROC_NAME, 0, NULL, &proc_fops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit seconds_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("A simple kernel module to report elapsed seconds");

module_init(seconds_init);
module_exit(seconds_exit);
