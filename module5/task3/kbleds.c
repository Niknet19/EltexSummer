/*
 * kbleds.c − Blink keyboard leds until the module is unloaded.(modified for > 4.15)
 */
#include <linux/module.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h> /* For fg_console, MAX_NR_CONSOLES */
#include <linux/kd.h>  /* For KDSETLED */
#include <linux/vt.h>
#include <linux/console_struct.h> /* For vc_cons */
#include <linux/vt_kern.h>
#include <linux/timer.h>

MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_LICENSE("GPL");
static struct timer_list my_timer;
static struct tty_driver *my_driver;
// char kbledstatus = 0;
static int _kbledstatus = 0;
// static int attr = 3;
static struct kobject *example_kobject;
static int led = 0;
#define BLINK_DELAY HZ / 3 // Медленнее для одного LED
#define CAPS_LOCK_ON 0x04
#define ALL_LEDS_ON 0x07
#define SYSFS_NAME "kbled"
#define ALL_LEDS_OFF 0x00
#define RESTORE_LEDS 0xFF
/*
 * Function my_timer_func blinks the keyboard LEDs periodically by invoking
 * command KDSETLED of ioctl() on the keyboard driver. To learn more on virtual
 * terminal ioctl operations, please see file:
 *     /usr/src/linux/drivers/char/vt_ioctl.c, function vt_ioctl().
 *
 * The argument to KDSETLED is alternatively set to 7 (thus causing the led
 * mode to be set to LED_SHOW_IOCTL, and all the leds are lit) and to 0xFF
 * (any value above 7 switches back the led mode to LED_SHOW_FLAGS, thus
 * the LEDs reflect the actual keyboard status). To learn more on this,
 * please see file:
 *     /usr/src/linux/drivers/char/keyboard.c, function setledstate().
 *
 */

static ssize_t led_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
    return sprintf(buf, "%d\n", led);
}

static ssize_t led_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
    int new_led = 0;
    sscanf(buf, "%d", &new_led);
    if (new_led < ALL_LEDS_OFF || new_led > ALL_LEDS_ON)
        return -EINVAL;
    led = new_led;
    return count;
}

static struct kobj_attribute foo_attribute = __ATTR(led, 0660, led_show,
                                                    led_store);

static int __init sys_init(void)
{
    int error = 0;

    pr_debug("Module initialized successfully \n");

    example_kobject = kobject_create_and_add(SYSFS_NAME,
                                             kernel_kobj);
    if (!example_kobject)
        return -ENOMEM;

    error = sysfs_create_file(example_kobject, &foo_attribute.attr);
    if (error)
    {
        pr_debug("failed to create the foo file in /sys/kernel/kbled \n");
    }

    return error;
}

static void __exit sys_exit(void)
{
    pr_debug("Module un initialized successfully \n");
    kobject_put(example_kobject);
}

// static void my_timer_func(unsigned long ptr)
static void my_timer_func(struct timer_list *ptr)
{
    // int *pstatus = (int *)ptr;
    int *pstatus = &_kbledstatus;
    if (*pstatus == led)
        *pstatus = ALL_LEDS_OFF;
    else
        *pstatus = led;
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED,
                            *pstatus);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init kbleds_init(void)
{
    sys_init();
    int i;
    printk(KERN_INFO "kbleds: loading\n");
    printk(KERN_INFO "kbleds: fgconsole is %x\n", fg_console);
    for (i = 0; i < MAX_NR_CONSOLES; i++)
    {
        if (!vc_cons[i].d)
            break;
        printk(KERN_INFO "poet_atkm: console[%i/%i] #%i, tty %lx\n", i,
               MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
               (unsigned long)vc_cons[i].d->port.tty);
    }
    printk(KERN_INFO "kbleds: finished scanning consoles\n");
    my_driver = vc_cons[fg_console].d->port.tty->driver;
    // printk(KERN_INFO "kbleds: tty driver magic %x\n", my_driver->magic);
    /*
     * Set up the LED blink timer the first time
     */

    // init_timer(&my_timer);
    timer_setup(&my_timer, my_timer_func, 0);
    // my_timer.function = my_timer_func;
    // my_timer.data = (unsigned long)&kbledstatus;
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
    return 0;
}
static void __exit kbleds_cleanup(void)
{
    sys_exit();
    printk(KERN_INFO "kbleds: unloading...\n");
    del_timer(&my_timer);
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED,
                            RESTORE_LEDS);
}
module_init(kbleds_init);
module_exit(kbleds_cleanup);