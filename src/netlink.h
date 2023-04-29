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
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <netlink/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/route/addr.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PAYLOAD 8692

struct netlink {
    struct nl_sock *sock;
    int family_id;
    int protocol;
    int pid;
};

struct netlink * initialize_netlink(struct netlink *nl, char *family_name);

int send_nl(struct netlink *nl, char *buffer, ssize_t msg_size, int message_type,
            int flags);

int recv_nl(struct netlink *nl, char *buf, int buffer_size, int flags);

void close_nl(struct netlink *nl);

