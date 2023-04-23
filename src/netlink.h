/*
    Python client for the Netlink interface.
    Copyright (C) 2023 Boaz Tene

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct netlink {
    char family_name[256];
    int protocol;
    struct nl_sock *sock;
    struct sockaddr_nl src_addr;
    struct sockaddr_nl dst_addr;
};

struct netlink * initialize_netlink(struct netlink *nl, char* family_name);

int connect_nl(struct netlink *nl);

int do_nl(struct netlink *nl, const void *attributes, __u8 attribute_type, __u8 command,
          ssize_t attr_size);

void close_nl(struct netlink *nl);

