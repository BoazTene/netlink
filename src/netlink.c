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
#include <netlink/socket.h>

#include <errno.h>

int create_socket();

/**
 * Creates a new netlink object.
 *
 * @param nl allocated buffer.
 * @param magic_number the magic protocol (same in kernel).
 */
struct netlink *initialize_netlink(struct netlink *nl, int protocol, int family_id, struct nla_policy *policies, int policies_len, int hdrlen) {
    
    nl->sock = (struct nl_sock *)nl_socket_alloc();

    nl->hdrlen = hdrlen;
    nl->protocol = protocol;
    nl->family_id = family_id;
    nl->policies_len = policies_len;
    nl->policies = policies;

    nl_connect(nl->sock, nl->protocol);

    return nl;
}

/**
 * Resolves a family id from the family_name.
 * 
 * !Note Only for generic netlink ofcourse.
 *
 * @param family_name The family name to resolve.
 * @returns the family id.
 */
int resolve_genl_family_id(char* family_name) {
	int family_id;
	struct nl_sock * sock = nl_socket_alloc();
	if (!sock) return -1;

	nl_connect(sock, NETLINK_GENERIC);
	family_id = genl_ctrl_resolve(sock, family_name);

	nl_socket_free(sock);

	return family_id;
}

void modify_cb(struct netlink *nl, enum nl_cb_type type, enum nl_cb_kind kind, void *callback, void * arg) {
	nl_socket_modify_cb(nl->sock, NL_CB_VALID, NL_CB_CUSTOM, callback, arg);
}

void parse_attr_nl(struct netlink *nl, struct nl_msg *msg, struct nlattr **attrs) {
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
     
    if (nlmsg_parse(nlh, nl->hdrlen, attrs, nl->policies_len, nl->policies) < 0) {
        printf("Error parsing message\n");
        return NULL;
    }
}

int add_membership_nl(struct netlink *nl, int group) {
	return nl_socket_add_memberships(nl->sock, group, 0);
}

int drop_membership_nl(struct netlink *nl, int group) {
	return nl_socket_drop_memberships(nl->sock, group, 0);
}

int send_nl(struct netlink *nl, struct nl_msg *msg) {
    int ret = nl_send_auto(nl->sock, msg);
    if (ret < 0) {
        fprintf(stderr, "Error: failed to send Netlink message\n");
        return -1;
    }
    return ret;
}

int recv_nl(struct netlink *nl)
{
    struct sockaddr_nl nla = {0}; // talking to kernel.
    struct nl_sock * sock = (struct nl_sock *)nl->sock;
        
    int ret = nl_recvmsgs_default(nl->sock);

    if (ret < 0) {
	        printf("Failed to receive netlink message: %s\n", nl_geterror(ret));
        fprintf(stderr, "Error: Failed to receive netlink message %d\n", ret);
        return -1;
    }

    return ret;
}

void close_nl(struct netlink *nl) { 
	if (nl->sock == NULL) {
		// already freed 
		return;
	}

	nl_socket_free(nl->sock); 
	nl->sock = NULL;
}
