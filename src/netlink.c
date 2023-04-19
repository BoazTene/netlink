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
struct netlink *initialize_netlink(struct netlink *nl, int magic_number) {
  nl->sock_fd = create_socket(magic_number);

  memset(&nl->src_addr, 0, sizeof(struct sockaddr_nl));
  nl->src_addr.nl_family = AF_NETLINK;
  nl->src_addr.nl_pid = getpid(); /* self pid */

  memset(&nl->dst_addr, 0, sizeof(struct sockaddr_nl));
  nl->dst_addr.nl_family = AF_NETLINK;
  nl->dst_addr.nl_pid = 0;    /* For Linux Kernel */
  nl->dst_addr.nl_groups = 0; /* unicast */

  return nl;
}

/**
 * Creates a new socket.
 *
 * @param magic_number the magic protocol
 */
int create_socket(int magic_number) {
  return socket(PF_NETLINK, SOCK_RAW, magic_number);
}

/**
 * Binds to the nl address.
 */
int connect_nl(struct netlink *nl) {
  int res =
      bind(nl->sock_fd, (struct sockaddr *)&nl->src_addr, sizeof(nl->src_addr));
  return res;
}

/**
 * Sends a message to tne netlink.
 *
 * @param message the message to send.
 * @return the number of bytes sent or -1 on error.
 */
int send_nl(struct netlink *nl, const char *message, ssize_t size) {
  if (nl->sock_fd < 0)
    return -1;

  struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(size));
  struct iovec iov;
  struct msghdr msg;

  memset(nlh, 0, NLMSG_SPACE(size));
  nlh->nlmsg_len = NLMSG_SPACE(size);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;

  strcpy(NLMSG_DATA(nlh), message);

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = (void *)&nl->dst_addr;
  msg.msg_namelen = sizeof(nl->dst_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  int res = sendmsg(nl->sock_fd, &msg, 0);
  free(nlh);

  return res;
}

/**
 * Recieves a message from the netlink.
 *
 * @param buffer the data will be stored.
 * @param size the size of the buffer.
 */
void recv_nl(struct netlink *nl, char *buffer, ssize_t size) {
  struct nlmsghdr *nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(size));
  struct iovec iov;
  struct msghdr msg;

  memset(nlh, 0, NLMSG_SPACE(size));
  nlh->nlmsg_len = NLMSG_SPACE(size);
  nlh->nlmsg_pid = getpid();
  nlh->nlmsg_flags = 0;

  iov.iov_base = (void *)nlh;
  iov.iov_len = nlh->nlmsg_len;
  memset(&msg, 0, sizeof(msg));
  msg.msg_name = (void *)&nl->dst_addr;
  msg.msg_namelen = sizeof(nl->dst_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  recvmsg(nl->sock_fd, &msg, 0);

  char *result = NLMSG_DATA(nlh);
  strncpy(buffer, result, size);
  free(nlh);
}

/**
 * Closes the netlink.
 */
void close_nl(struct netlink *nl) { close(nl->sock_fd); }
