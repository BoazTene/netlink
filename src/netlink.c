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

/**
 * Creates a new netlink object.
 *
 * @param nl allocated buffer.
 * @param protocol such as (NETLINK_GENERIC, NETLINK_ROUTE)
 * @param family_id the family's identifier.
 * @param policies attribute's policies of the family.
 * @param policies_len number of policies.
 */
struct netlink *initialize_netlink(struct netlink *nl, int protocol, int family_id, struct nla_policy *policies, int policies_len, int hdrlen) {
    
    nl->sock = (struct nl_sock *)nl_socket_alloc();

    nl->hdrlen = hdrlen;
    nl->protocol = protocol;
    nl->family_id = family_id;
    nl->policies_len = policies_len;
    nl->policies = malloc(sizeof(struct nla_policy) * policies_len);
    for (int i = 0; i < policies_len; i++) {
	    nl->policies[i] = policies[i];
    }

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

/**
 * Resolves a group id from a family name and group name. 
 * 
 * !Note Only for generic netlink ofcourse.
 *
 * @param family_name The family name to resolve.
 * @param group_name The group_name to resolve.
 * @returns the group id.
 */
int resolve_genl_group_id(char* family_name, char* group_name) {
	int family_id;
	struct nl_sock * sock = nl_socket_alloc();
	if (!sock) return -1;

	nl_connect(sock, NETLINK_GENERIC);
	family_id = genl_ctrl_resolve_grp(sock, family_name, group_name);

	nl_socket_free(sock);

	return family_id;
}



/**
 * Modifies callbacks.
 * Note that these callbacks will get called when a message arrives.
 *
 * @param nl netlink object.
 * @param type callback's type.
 * @param kind callback's kind.
 * @param arg arguments to add when calling the callback.
 */
void modify_cb(struct netlink *nl, enum nl_cb_type type, enum nl_cb_kind kind, void *callback, void *arg) {
	nl_socket_modify_cb(nl->sock, NL_CB_VALID, NL_CB_CUSTOM, callback, arg);
}

/**
 * Parses attributes from a message.
 *
 * @param nl netlink object.
 * @param msg message to parse.
 * @param attrs attributes array in length of 1+MAX_ATTRIBUTE, the parsed attributes will be put there.
 */
void parse_attr_nl(struct netlink *nl, struct nl_msg *msg, struct nlattr **attrs) {
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    int ret;

    struct nlattr* nla = nlmsg_attrdata(nlh, 4);
   
    if ((ret = nlmsg_parse(nlh, nl->hdrlen, attrs, nl->policies_len, nl->policies)) < 0) {
        printf("Error parsing message %s\n", nl_geterror(ret));

    }
}

/**
 * Disables the seq check for the socket.
 *
 * @param nl netlink object.
 */
void disable_seq_check(struct netlink *nl) {
	nl_socket_disable_seq_check(nl->sock);
}

/**
 * Adds a new memebership to a multicast group.
 *
 * @param nl netlink object.
 * @param group multicast's group.
 * @return return code zero upon success.
 */
int add_membership_nl(struct netlink *nl, int group) {
	return nl_socket_add_memberships(nl->sock, group, 0);
}

/**
 * Drops a memebership from a multicast group.
 *
 * @param nl netlink object.
 * @param group multicast group.
 * @return return code, zero upon success.
 */
int drop_membership_nl(struct netlink *nl, int group) {
	return nl_socket_drop_memberships(nl->sock, group, 0);
}

/**
 * Sends a nl message.
 *
 * @param nl netlink object.
 * @param msg message to send.
 * @return return code, zero upon success
 */
int send_nl(struct netlink *nl, struct nl_msg *msg) {
    int ret = nl_send_auto(nl->sock, msg);
    if (ret < 0) {
        fprintf(stderr, "Error: failed to send Netlink message\n");
        return -1;
    }
    return ret;
}

/**
 * Recieves a message.
 *
 * @param nl netlink object
 * @return return code, zero upon success.
 */
int recv_nl(struct netlink *nl)
{
    int ret = nl_recvmsgs_default(nl->sock);

    if (ret < 0) {
	        printf("Failed to receive netlink message: %s\n", nl_geterror(ret));
        	fprintf(stderr, "Error: Failed to receive netlink message %d\n", ret);
        return -1;
    }

    return ret;
}

/**
 * Closes netlink connection and frees the socket.
 *
 * @param nl netlink object.
 */
void close_nl(struct netlink *nl) { 
	if (nl->sock == NULL) {
		// already freed 
		return;
	}

	nl_socket_free(nl->sock); 
	nl->sock = NULL;
}
