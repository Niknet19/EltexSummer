#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROCFS_NAME "hellokernel"
#define PROCFS_MAX_SIZE 1024

static int len, temp;
static char *msg;

ssize_t read_proc(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
    if (count > temp)
    {
        count = temp;
    }
    temp = temp - count;
    copy_to_user(buf, msg, count);
    if (count == 0)
        temp = len;
    return count;
}

ssize_t write_proc(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
    size_t bytes_to_write;
    bytes_to_write = min(count, (size_t)(PROCFS_MAX_SIZE - 1));
    copy_from_user(msg, buf, bytes_to_write);
    len = bytes_to_write;
    temp = len;
    return bytes_to_write;
}

static const struct proc_ops proc_fops = {
    .proc_read = read_proc,
    .proc_write = write_proc,
};

void create_new_proc_entry(void)
{ // use of void for no arguments is compulsory now
    proc_create(PROCFS_NAME, 0, NULL, &proc_fops);
    msg = kmalloc(PROCFS_MAX_SIZE * sizeof(char), GFP_KERNEL);
}

int proc_init(void)
{
    create_new_proc_entry();
    return 0;
}

void proc_cleanup(void)
{
    remove_proc_entry(PROCFS_NAME, NULL);
    kfree(msg);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Feoktistov Nikita");
module_init(proc_init);
module_exit(proc_cleanup);