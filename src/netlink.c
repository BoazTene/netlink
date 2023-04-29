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

    nl->pid = getpid();
    nl->protocol = NETLINK_GENERIC;
    nl_connect(nl->sock, nl->protocol);
    
    nl->family_id = genl_ctrl_resolve(nl->sock, family_name);

    return nl;
}

int send_nl(struct netlink *nl, char *buffer, ssize_t msg_size, int message_type,
            int flags) {
    struct nlmsghdr *nlh;
    struct nl_msg *msg;
    
    msg = nlmsg_alloc();
    if (!msg) {
        fprintf(stderr, "Error: failed to allocate Netlink message\n");
        return -1;
    }
    char name[100];
    strcpy(name, "1234");
    
    if (!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, nl->family_id, 0, 0, 1, 1)) printf("fucking problem\n");
    
    nla_put(msg, 1, strlen(name)+1,name); 
       
    int ret = nl_send_auto(nl->sock, msg);
    if (ret < 0) {
        fprintf(stderr, "Error: failed to send Netlink message\n");
        return -1;
    }

    nlmsg_free(msg);

    return ret;
}

int recv_nl(struct netlink *nl, char *buf, int buffer_size, int flags)
{
    struct nlmsghdr *nlh;
    struct nl_msg *msg;

    msg = nlmsg_alloc();
    if (!msg) {
        fprintf(stderr, "Error: Failed to allocated memory for msg\n");
        return -1;
    }

    int ret = nl_recvmsgs(nl->sock, msg);
    if (ret < 0) {
        fprintf(stderr, "Error: Failed to receive netlink message\n");
        nlmsg_free(msg);
        return -1;
    }

    nlh = nlmsg_hdr(msg);

    if (nlh->nlmsg_type == NLMSG_ERROR) {
        fprintf(stderr, "Error: received Netlink error message\n");
        nlmsg_free(msg);
        return -1;
    }

    if (nlh->nlmsg_len > sizeof(struct nlmsghdr)) {
        memcpy(buf, NLMSG_DATA(nlh), buffer_size);
    }


    nlmsg_free(msg);

    return 0;
}

void close_nl(struct netlink *nl) { 
	if (nl->sock == NULL) {
		// already freed 
		return;
	}

	nl_socket_free(nl->sock); 
	nl->sock = NULL;
}
