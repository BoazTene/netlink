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
    if (!nl->sock) printf("bad shit\n");
    
    int res =nl_connect(nl->sock, 31);
    printf("res: %d\n", res);
    nl->protocol = genl_ctrl_resolve(nl->sock, "echo");
    printf("protocol: %d\n", genl_ctrl_resolve(nl->sock, "echo"));

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

	struct ifinfomsg ifi = {

		.ifi_family = AF_INET,

		.ifi_index = nl->protocol,

	};

	printf("protocol: %d\n", nl->protocol);
	printf("nl dst: %d\n", nl->dst_addr.nl_pid);
    nlmsg_append(msg, &ifi, sizeof(ifi), NLMSG_ALIGNTO);
    struct nl_addr *addr;
    addr = nl_addr_build(AF_NETLINK, &nl->dst_addr.nl_pid, sizeof(nl->dst_addr.nl_pid));
   
    NLA_PUT_U32(msg, IFLA_MTU, 4096); 
    NLA_PUT_ADDR(msg, IFLA_ADDRESS, addr);
    NLA_PUT_STRING(msg, attribute_type, attribute);
    

    int res = nl_send_auto(nl->sock, msg);
    
    nl_addr_put(addr);
    nlmsg_free(msg);

    return res;

    nla_put_failure:

        nlmsg_free(msg);

        return NULL;
}


void close_nl(struct netlink *nl) 
{
    nl_socket_free(nl->sock);
}
