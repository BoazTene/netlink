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

#include "netlink_class.h"
#include "message.h"
#include "attribute.h"
#include "attribute_policy.h"
#include <Python.h>

#define resolve_genl_family_id_docs "A static method that resolve the family id of an generic netlink.\n@param family_name The family name\n@return The family id"

static PyObject *netlink_resolve_genl_family_id(PyObject *cls, PyObject *args) {
	char *family_name;
	
	if (!PyArg_ParseTuple(args, "s", &family_name) ) {
		return NULL;
	}

	int family_id = resolve_genl_family_id(family_name);

	return PyLong_FromLong(family_id);
}

#define resolve_genl_group_id_docs "A static method that resolve the group id of an generic netlink multicast group.\n@param family_name The family name\n@param group_name The group name\n@return The group id"

static PyObject *netlink_resolve_genl_group_id(PyObject *cls, PyObject *args) {
	char *family_name;
	char *group_name;
	
	if (!PyArg_ParseTuple(args, "ss", &family_name, &group_name) ) {
		return NULL;
	}

	int group_id = resolve_genl_group_id(family_name, group_name);

	return PyLong_FromLong(group_id);
}

#define send_docs "Sends a message.\n@param message The message to send"

static PyObject *netlink_send(NetLink *self, PyObject *args) {
    Message *message;

    if (!PyArg_ParseTuple(args, "O!", &MessageType, &(message))) {
        return NULL;
    }

    send_nl(self->netlink, message->msg);

    Py_RETURN_NONE;
}

#define parse_docs "Parses message's attributes.\n@param message message to parse\n@return list of attributes (list[Attribute])."

static PyObject *netlink_parse(NetLink *self, PyObject *args) {
    Message *message;

    if (!PyArg_ParseTuple(args, "O!", &MessageType, &(message))) {
        return NULL;
    }
    
    struct nlattr* attrs[self->netlink->policies_len+1];

    parse_attr_nl(self->netlink, message->msg, attrs);
    
    PyObject * attribute_list = PyList_New(0);
   
    for (int i = 0; i < self->netlink->policies_len+1; i++) {
	    if (!attrs[i]) continue; // checks if attributes exists
				     
	    Attribute *attribute = PyObject_New(Attribute, &AttributeType);
	    attribute->len = nla_len(attrs[i]);
	    attribute->data = nla_data(attrs[i]);
	    attribute->type = nla_type(attrs[i]);

            PyList_Append(attribute_list, (PyObject *) attribute);
    }

    return attribute_list;
}

#define get_family_id_docs "Getter for the family id.\n@return the family id"

static PyObject *netlink_get_family_id(NetLink *self, PyObject *args) {
	return PyLong_FromLong(self->netlink->family_id);
}

#define disable_seq_check_docs "Disables the sequential check."

static PyObject *netlink_disable_seq(NetLink *self, PyObject *args) {
	disable_seq_check(self->netlink);

	Py_RETURN_NONE;
}

/**
 * Callback handler.
 * Used as a middle man between the cb and the python.
 *
 * @param msg The recieved msg.
 * @param callback The callback to call.
 * @return error code.
 */
static int * cb_callback_handler(struct nl_msg *msg, PyObject *callback) {
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	PyObject *arglist;

	Message *message = PyObject_New(Message, &MessageType);
	message->msg = msg;
	arglist = PyTuple_Pack(1, message);
	
	PyObject_CallObject(callback, arglist);
	
	Py_DECREF(arglist);
	PyGILState_Release(gstate);

	return 0;
}

#define modify_cb_docs "Modifies the cb of the netlink.\n@param kind kind of the object (CB_Kind).\n@param type type of the object (CB_Type)\n@param callback The callback to set"

static PyObject *netlink_modify_cb(NetLink *self, PyObject *args) {
    PyObject *callback;
    int kind;
    int type;

    if (PyArg_ParseTuple(args, "iiO:set_callback", &kind, &type,  &callback)) {
	if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    } else {
	   return NULL;
    } 
    Py_INCREF(callback);

    modify_cb(self->netlink, type, kind, cb_callback_handler, callback); 

    Py_RETURN_NONE;
}

#define add_membership_docs "Adds a membership to a multicast group.\n@param group multicast group."

static PyObject *netlink_add_membership(NetLink *self, PyObject *args) {
    int group;

    if (!PyArg_ParseTuple(args, "i", &group)) {
	   return NULL;
    } 
    
    add_membership_nl(self->netlink, group);
    
    Py_RETURN_NONE;
}

#define drop_membership_docs "Drops membership to a multicast group.\n@param group multicast group."

static PyObject *netlink_drop_membership(NetLink *self, PyObject *args) {
    int group;

    if (!PyArg_ParseTuple(args, "i", &group)) {
	   return NULL;
    } 
    
    drop_membership_nl(self->netlink, group);
    
    Py_RETURN_NONE;
}

#define recv_docs "Receives a message.\nThe appropriate cb will be called."

static PyObject *netlink_recv(NetLink *self, PyObject *args) {
    int ret = recv_nl(self->netlink);

    if (ret != 0) {
	    return NULL;
    }

    Py_RETURN_NONE;
}

#define close_docs "Closes netlink connection.\n"

static PyObject *netlink_close(NetLink *self, PyObject *args) {
    if (self->netlink != NULL) {
        close_nl(self->netlink);
        Py_RETURN_NONE;
    }

    return NULL;
}

static PyObject *NetLink_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
    NetLink *self;

    self = (NetLink *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void NetLink_dealloc(NetLink *self) {
    if (self->netlink != NULL) {
        close_nl(self->netlink);
        free(self->netlink->policies);
        free(self->netlink);
    }

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static struct nla_policy * get_policies(AttributePolicy **policies, int policies_len, struct nla_policy *nla_policies) {
    for (int i = 0; i < policies_len; i++) {
	 nla_policies[i] = policies[i]->policy;
    }

    return nla_policies;
}

static void NetLink_init(NetLink *self, PyObject *args, PyObject *kwds) {
    PyObject *policies_list;
    int family_id;
    int protocol;
    int hdrlen;

    if (!PyArg_ParseTuple(args, "iiiO", &family_id, &protocol, &hdrlen, &policies_list)) return;

    if (!PyList_Check(policies_list)) {
	    PyErr_SetString(PyExc_TypeError, "Attribute must be a list");
	    return;
    }

    int policies_len = PyList_Size(policies_list);
    AttributePolicy ** policies = (AttributePolicy **) malloc(sizeof(AttributePolicy *) * policies_len); 

    if (policies == NULL) {
        return;
    }

    for (int i = 0; i < policies_len; i++) {
	    PyObject *item = PyList_GetItem(policies_list, i);

	    if (!PyObject_IsInstance(item, (PyObject *) &AttributePolicyType)) {
		    PyErr_SetString(PyExc_TypeError, "List must contain AttributePolicy");
		    Py_DECREF(item);
		    return;
	    }

	    policies[i] = (AttributePolicy *) item; 
    }

    self->netlink = (struct netlink *)malloc(sizeof(struct netlink));

    struct nla_policy nla_policies[policies_len];

    get_policies(policies, policies_len, nla_policies);


    self->netlink = initialize_netlink(self->netlink, protocol, family_id, &nla_policies[0], policies_len, hdrlen);

    if (!self->netlink->sock) {
        PyErr_SetString(PyExc_ConnectionRefusedError,
                        "Couldn't connect to netlink.");
    }
}

static PyMemberDef NetLink_members[] = {
    {"__netlink", T_OBJECT_EX, offsetof(NetLink, netlink), 0, "The netlink."},
    {NULL} /* Sentinel */
};

static PyMethodDef NetLink_methods[] = {
    {"send", (PyCFunction) netlink_send, METH_VARARGS, send_docs},
    {"recv", (PyCFunction) netlink_recv, METH_VARARGS, recv_docs},
    {"get_family_id", (PyCFunction)netlink_get_family_id, METH_VARARGS, get_family_id_docs},
    {"disable_seq_check", (PyCFunction)netlink_disable_seq, METH_VARARGS, disable_seq_check_docs},
    {"close", (PyCFunction) netlink_close, METH_VARARGS,
     close_docs},
    {"modify_cb", (PyCFunction) netlink_modify_cb, METH_VARARGS, modify_cb_docs},
    {"parse_message_attributes", (PyCFunction) netlink_parse, METH_VARARGS, parse_docs},
    {"resolve_genl_family_id", (PyCFunction) netlink_resolve_genl_family_id, METH_VARARGS | METH_CLASS, resolve_genl_family_id_docs},
    {"resolve_genl_group_id", (PyCFunction) netlink_resolve_genl_group_id, METH_VARARGS | METH_CLASS, resolve_genl_group_id_docs},
    {"add_membership", (PyCFunction) netlink_add_membership, METH_VARARGS, add_membership_docs},
    {"drop_membership", (PyCFunction) netlink_drop_membership, METH_VARARGS, drop_membership_docs},
    {NULL} /* Sentinel */
};

PyTypeObject NetLinkType = {
    PyVarObject_HEAD_INIT(NULL, 0) "netlink.NetLink", /* tp_name */
    sizeof(NetLink),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)NetLink_dealloc,                      /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_reserved */
    0,                                                /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash  */
    0,                                                /* tp_call */
    0,                                                /* tp_str */
    0,                                                /* tp_getattro */
    0,                                                /* tp_setattro */
    0,                                                /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                               /* tp_flags */
    "Client implmentation of the netlink kenrel interface.", /* tp_doc */
    0,                                                       /* tp_traverse */
    0,                                                       /* tp_clear */
    0,                      /* tp_richcompare */
    0,                      /* tp_weaklistoffset */
    0,                      /* tp_iter */
    0,                      /* tp_iternext */
    NetLink_methods,        /* tp_methods */
    NetLink_members,        /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)NetLink_init, /* tp_init */
    0,                      /* tp_alloc */
    NetLink_new,            /* tp_new */
};
