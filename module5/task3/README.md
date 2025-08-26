# Модуль ядра для управления мигания лампочек на клавиатуре
Управление происходит через sysfs. Для управления нужно записать определенное значение в в /sys/kernel/kbled/led
- 0 - все лампочки выключены.
- 4 - мигает caps lock.
- 2 - мигает numlock
- 1 - мигает scrolllock
- 7 - мигают все лампочки

## Пример работы модуля:  
`sudo echo "4" > /sys/kernel/kbled/led`   

![video_2025-08-26_18-32-39 (1)](https://github.com/user-attachments/assets/448e7ada-0928-4d62-8163-41bde2bd2bf7)


## Другой пример:

![video_2025-08-26_18-32-51 (2)](https://github.com/user-attachments/assets/04662106-a81f-4669-b861-3a8899219b29)   
Для этого нужно изменить код модуля ядра.   
Нужно переписать функцию my_timer_func вот так:
```
#define CAPS_LOCK_ON 0x04
#define ALL_LEDS_ON 0x07
#define ONE_LED_ON 0x01
#define ALL_LEDS_OFF 0x00

...

static void my_timer_func(struct timer_list *ptr)
{
    // int *pstatus = (int *)ptr;
    int *pstatus = &led;
    if (*pstatus == CAPS_LOCK_ON)
        *pstatus = ONE_LED_ON;
    else
        *pstatus = *pstatus << 1;
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED,
                            *pstatus);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}
```
