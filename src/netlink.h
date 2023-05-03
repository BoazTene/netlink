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
#include "argument_policy.h"
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


#ifndef NETLINK_H
#define NETLINK_H
#define MAX_PAYLOAD 8692

struct netlink {
    struct nl_sock *sock;
    int hdrlen;
    int family_id;
    int protocol;
    int policies_len;
    struct nla_policy *policies;
};

struct netlink * initialize_netlink(struct netlink *nl, int protocol, int family_id, struct nla_policy *policies, int policies_len, int hdrlen);

int send_nl(struct netlink *nl, struct nl_msg * msg);

int recv_nl(struct netlink *nl);

void parse_attr_nl(struct netlink *nl, struct nl_msg *msg, struct nlattr ** attrs);

void modify_cb(struct netlink *nl, enum nl_cb_type type, enum nl_cb_kind kind, void *callback, void * arg);

int add_membership_nl(struct netlink *nl, int group);

int drop_membership_nl(struct netlink *nl, int group);

void close_nl(struct netlink *nl);
#endif

