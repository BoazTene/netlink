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

#ifndef NETLINK_H
#define NETLINK_H

#include "attribute_policy.h"
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

/**
 * Represents a connection to a netlink family.
 *
 * sock -> The connected socket.
 * hdrlen -> Size of the header length.
 * family_id -> The family's unique id.
 * protocol -> The protocol to use.
 * policies_len -> The number of attributes.
 * policies -> An array of the attribute's policies.
 */
struct netlink {
    struct nl_sock *sock;
    int hdrlen;
    int family_id;
    int protocol;
    int policies_len;
    struct nla_policy *policies;
};

/**
 * Creates a new netlink object.
 *
 * @param nl allocated buffer.
 * @param protocol such as (NETLINK_GENERIC, NETLINK_ROUTE)
 * @param family_id the family's identifier.
 * @param policies attribute's policies of the family.
 * @param policies_len number of policies.
 */
struct netlink * initialize_netlink(struct netlink *nl, int protocol, int family_id, struct nla_policy *policies, int policies_len, int hdrlen);

/**
 * Resolves a family id from the family_name.
 * 
 * !Note Only for generic netlink ofcourse.
 *
 * @param family_name The family name to resolve.
 * @returns the family id.
 */
int resolve_genl_family_id(char* family_name);


/**
 * Resolves a group id from a family name and group name. 
 * 
 * !Note Only for generic netlink ofcourse.
 *
 * @param family_name The family name to resolve.
 * @param group_name The group_name to resolve.
 * @returns the group id.
 */
int resolve_genl_group_id(char* family_name, char* group_name);

/**
 * Sends a nl message.
 *
 * @param nl netlink object.
 * @param msg message to send.
 * @return return code, zero upon success
 */
int send_nl(struct netlink *nl, struct nl_msg * msg);

/**
 * Recieves a message.
 *
 * @param nl netlink object
 * @return return code, zero upon success.
 */
int recv_nl(struct netlink *nl);

/**
 * Parses attributes from a message.
 *
 * @param nl netlink object.
 * @param msg message to parse.
 * @param attrs attributes array in length of 1+MAX_ATTRIBUTE, the parsed attributes will be put there.
 */
void parse_attr_nl(struct netlink *nl, struct nl_msg *msg, struct nlattr ** attrs);

/**
 * Modifies callbacks.
 * Note that these callbacks will get called when a message arrives.
 *
 * @param nl netlink object.
 * @param type callback's type.
 * @param kind callback's kind.
 * @param arg arguments to add when calling the callback.
 */
void modify_cb(struct netlink *nl, enum nl_cb_type type, enum nl_cb_kind kind, void *callback, void * arg);

/**
 * Adds a new memebership to a multicast group.
 *
 * @param nl netlink object.
 * @param group multicast's group.
 * @return return code zero upon success.
 */
int add_membership_nl(struct netlink *nl, int group);

/**
 * Drops a memebership from a multicast group.
 *
 * @param nl netlink object.
 * @param group multicast group.
 * @return return code, zero upon success.
 */
int drop_membership_nl(struct netlink *nl, int group);

/**
 * Disables the seq check for the socket.
 *
 * @param nl netlink object.
 */
void disable_seq_check(struct netlink *nl);

/**
 * Closes netlink connection and frees the socket.
 *
 * @param nl netlink object.
 */
void close_nl(struct netlink *nl);

#endif

