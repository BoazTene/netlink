obj-m += netlink_example.o

netlink_example-objs := main.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -f *.o *.ko *.mod.c *.mod *.symvers *.order

ifneq ($(KERNELRELEASE),)
# We were called by kbuild

ifneq ($(strip $(netlink_example-objs)),)
obj-m += netlink_example.o
endif

else
# We were called from command line

ifneq ($(strip $(netlink_example-objs)),)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
else
all: built-in
endif

endif

