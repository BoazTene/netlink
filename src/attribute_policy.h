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

#ifndef ATTRIBUTE_POLICY_H
#define ATTRIBUTE_POLICY_H

#include "Python.h"
#include <structmember.h>
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


/**
 * Represents NetLink class.
 */
typedef struct {
    PyObject_HEAD
    struct nla_policy policy;
} AttributePolicy; 

extern PyTypeObject AttributePolicyType;

#endif
