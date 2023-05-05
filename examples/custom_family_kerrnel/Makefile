# Python client for the Netlink interface. \
Copyright (C) 2023 Boaz Tene \
\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 3 of the License, or \
any later version. \
\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details. \
\
You should have received a copy of the GNU General Public License \
along with this program.  If not, see <https://www.gnu.org/licenses/>. 


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

