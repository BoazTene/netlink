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
#include "netlink.h"

#include <errno.h>

int create_socket();

/**
 * Creates a new netlink object.
 *
 * @param nl allocated buffer.
 * @param magic_number the magic protocol (same in kernel).
 */
struct netlink *initialize_netlink(struct netlink *nl, char *family_name) {
    nl->sock = (struct nl_sock *)nl_socket_alloc();
    memcpy(nl->family_name, family_name, 256);
    nl->protocol = genl_ctrl_resolve(nl->sock, family_name);

    memset(&nl->src_addr, 0, sizeof(struct sockaddr_nl));
    nl->src_addr.nl_family = AF_NETLINK;
    nl->src_addr.nl_pid = getpid(); /* self pid */

    memset(&nl->dst_addr, 0, sizeof(struct sockaddr_nl));
    nl->dst_addr.nl_family = AF_NETLINK;
    nl->dst_addr.nl_pid = 0;    /* For Linux Kernel */
    nl->dst_addr.nl_groups = 0; /* unicast */

    return nl;
}

int do_nl(struct netlink *nl, const void *attribute, uint8_t attribute_type,
          uint8_t command, ssize_t attr_size) {
    struct nl_msg *msg = (struct nl_msg *)nlmsg_alloc();
    if (!msg) {
        return -1;
    }

    nla_put_u8(msg, GENL_HDRLEN, command);
    struct nlattr *attrs[4096];
    int attrlen = 0;

    attrlen += NLA_HDRLEN + attr_size;
    attrs[attrlen++] = (struct nlattr *)GENL_HDRLEN;
    nla_put_u8(msg, attrlen, attribute);

    /* Set the netlink header */
    nlmsg_set_proto(msg, nl->protocol);
    nlmsg_set_flags(msg, NLM_F_REQUEST);
    nlmsg_set_type(msg, NLMSG_DONE);
    nlmsg_set_payload(msg, attrlen);

    int res = nl_send_auto(nl->sock, msg);
    
    nlmsg_free(msg);

    return res;
}


void close_nl(struct netlink *nl) 
{
    nl_socket_free(nl->sock);
}
