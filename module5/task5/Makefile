obj-m += netlink_kernel.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
user:
	gcc netlink_user.c -o netlink_user
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean