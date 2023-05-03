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
#include "argument_policy.h"

#include <Python.h>

static PyObject *netlink_resolve_genl_family_id(PyObject *cls, PyObject *args) {
	char *family_name;
	
	if (!PyArg_ParseTuple(args, "s", &family_name) ) {
		return NULL;
	}

	int family_id = resolve_genl_family_id(family_name);

	return PyLong_FromLong(family_id);
}

static PyObject *netlink_send(NetLink *self, PyObject *args) {
    Message *message;

    if (!PyArg_ParseTuple(args, "O!", &MessageType, &(message))) {
        return NULL;
    }

    send_nl(self->netlink, message->msg);

    Py_RETURN_NONE;
}

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

            PyList_Append(attribute_list, attribute);
    }

    return attribute_list;
}

static PyObject *netlink_get_family_id(NetLink *self, PyObject *args) {
	return PyLong_FromLong(self->netlink->family_id);
}

static int * cb_callback_handler(struct nl_msg *msg, PyObject *callback) {
	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();
	PyObject *arglist;
	PyObject *result;

	Message *message = PyObject_New(Message, &MessageType);
	message->msg = msg;
	arglist = PyTuple_Pack(1, message);
	
	result = PyObject_CallObject(callback, arglist);
	
	Py_DECREF(arglist);
	PyGILState_Release(gstate);

	return 0;
}

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

static PyObject *netlink_add_membership(NetLink *self, PyObject *args) {
    int group;

    if (!PyArg_ParseTuple(args, "i", &group)) {
	   return NULL;
    } 
    
    add_membership_nl(self->netlink, group);
    
    Py_RETURN_NONE;
}

static PyObject *netlink_drop_membership(NetLink *self, PyObject *args) {
    int group;

    if (!PyArg_ParseTuple(args, "i", &group)) {
	   return NULL;
    } 
    
    drop_membership_nl(self->netlink, group);
    
    Py_RETURN_NONE;
}


static PyObject *netlink_recv(NetLink *self, PyObject *args) {
    int ret = recv_nl(self->netlink);

    if (ret != 0) {
	    return NULL;
    }

    Py_RETURN_NONE;
}

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

static struct nla_policy * get_policies(ArgumentPolicy **policies, int policies_len) {
    struct nla_policy nla_policies[policies_len];

    for (int i = 0; i < policies_len; i++) {
        nla_policies[i] = policies[i]->policy;
    }

    return  nla_policies;
}

static void NetLink_init(NetLink *self, PyObject *args, PyObject *kwds) {
    PyObject *policies_list;
    int family_id;
    int protocol;
    int hdrlen;

    if (!PyArg_ParseTuple(args, "iiiO", &family_id, &protocol, &hdrlen, &policies_list)) return;

    if (!PyList_Check(policies_list)) {
	    PyErr_SetString(PyExc_TypeError, "Argument must be a list");
	    return;
    }

    int policies_len = PyList_Size(policies_list);
    ArgumentPolicy ** policies = (ArgumentPolicy **) malloc(sizeof(ArgumentPolicy *) * policies_len); 

    if (policies == NULL) {
        return;
    }

    for (int i = 0; i < policies_len; i++) {
	    PyObject *item = PyList_GetItem(policies_list, i);

	    if (!PyObject_IsInstance(item, &ArgumentPolicyType)) {
		    PyErr_SetString(PyExc_TypeError, "List must contain ArgumentPolicy");
		    Py_DECREF(item);
		    return;
	    }

	    policies[i] = (ArgumentPolicy *) item; 
    }

    self->netlink = (struct netlink *)malloc(sizeof(struct netlink));

    struct nla_policy *  nla_policies = get_policies(policies, policies_len);

    self->netlink = initialize_netlink(self->netlink, protocol, family_id, nla_policies, policies_len, hdrlen);

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
    {"send", netlink_send, METH_VARARGS, "Send method"},
    {"recv", netlink_recv, METH_VARARGS, "Recv method"},
    {"get_family_id", netlink_get_family_id, METH_VARARGS, "Gets the family id.\n@return the family id"},
    {"close", (PyCFunction)netlink_close, METH_VARARGS,
     "closes the connection."},
    {"modify_cb", netlink_modify_cb, METH_VARARGS, "modifies the callbacks"},
    {"parse_message_attributes", netlink_parse, METH_VARARGS, "parses a message"},
    {"resolve_genl_family_id", netlink_resolve_genl_family_id, METH_VARARGS | METH_CLASS, "Resolves family id from a generic netlink family name."},
    {"add_membership", netlink_add_membership, METH_VARARGS, "Adds a memebership to multicast group"},
    {"drop_membership", netlink_drop_membership, METH_VARARGS, "Drops a membership to multicast graoup"},
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
