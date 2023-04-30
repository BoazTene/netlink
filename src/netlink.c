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
struct netlink *initialize_netlink(struct netlink *nl, char *family_name, struct nla_policy *policies, int policies_len) {

    nl->sock = (struct nl_sock *)nl_socket_alloc();

    nl->pid = getpid();
    nl->protocol = NETLINK_GENERIC;
    nl_connect(nl->sock, nl->protocol);
    nl->family_id = genl_ctrl_resolve(nl->sock, family_name);
    nl->policies = malloc(sizeof(struct nla_policy) * policies_len);
    memcpy(nl->policies, policies, sizeof(struct nla_policy) *policies_len);
    nl->policies_len = policies_len;

    nl_socket_modify_cb(nl->sock, NL_CB_VALID, NL_CB_CUSTOM, recv_valid_message, nl);

    return nl;
}

int recv_valid_message(struct nl_msg *msg, struct netlink *nl) {
    struct nlattr* attrs[nl->policies_len+1];
    struct nlmsghdr *nlh = nlmsg_hdr(msg);

    if (nlmsg_parse(nlh, 0, attrs, nl->policies_len, nl->policies) < 0) {
        printf("Error parsing message\n");
        return -1;
    }

    printf("Parsing success\n"); 
    return 0;
}

int send_nl(struct netlink *nl, struct nl_msg *msg) {
    int ret = nl_send_auto(nl->sock, msg);
    if (ret < 0) {
        fprintf(stderr, "Error: failed to send Netlink message\n");
        return -1;
    }
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

    int ret = nl_recvmsgs_default(nl->sock, msg);
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
