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
  
#include "message.h"


#define reserve_docs "Reserves room for additional data at the tail of the an existing netlink message. Eventual padding required will be zeroed out.\n@param len length of additional data to reserve room for\n@param pad number of bytes to align data to\n@return null"


static PyObject * message_reserve(Message *self, PyObject *args) {
    int len;
    int pad;

    if (!PyArg_ParseTuple(args, "ii", &len, &pad)) {
        return NULL;
    }

    nlmsg_reserve(self->msg, len, pad);

    Py_RETURN_NONE;
}

#define get_bytes_docs "@return message in bytes (headers + payload)"

static PyObject * message_get_bytes(Message *self, PyObject *args) {
	struct nlmsghdr *nlh = nlmsg_hdr(self->msg);
	int message_len = NLMSG_LENGTH(nlh->nlmsg_len);

	PyObject * message_bytes = PyBytes_FromStringAndSize((char *) nlh, message_len);

	return message_bytes;
} 

#define append_docs "Append data to tail of a netlink message\n@param data data to add\n@param len length of data\n@param pad Number of bytes to align data to"

static PyObject * message_append(Message *self, PyObject *args) {
    Py_buffer buffer;
    int pad;

    if (!PyArg_ParseTuple(args, "y*i", &buffer, &pad)) {
        return NULL;
    }

    if (nlmsg_append(self->msg, (char *) buffer.buf, buffer.len, pad) > 0) {
        return NULL;
    }

    PyBuffer_Release(&buffer);

    Py_RETURN_NONE; 
}

#define nla_put_docs "Add a unspecific attribute to netlink message.\n@param attribute_type Attribute type.\n@param data Pointer to data to be used as attribute payload."

static PyObject * message_nla_put(Message *self, PyObject *args) {
    Py_buffer buffer;
    int attribute_type;
    
    if (!PyArg_ParseTuple(args, "y*i", &buffer, &attribute_type)) {
            return NULL;
    }

    nla_put(self->msg, attribute_type, buffer.len, (void *) buffer.buf);

    PyBuffer_Release(&buffer);

    Py_RETURN_NONE;
}

#define from_bytes_docs "A static method that creates a message object from bytes.\n@param bytes A full message bytes (header+payload)\n@return A new Message"

static PyObject *message_from_bytes(PyObject *cls,  PyObject *args) {
	Py_buffer buffer;

	if (!PyArg_ParseTuple(args, "y*", &buffer)) {
		return NULL;
	}
	
	Message * message = PyObject_New(Message, &MessageType);
	message->msg = nlmsg_alloc();
	void *data = nlmsg_put(message->msg, NL_AUTO_PORT, NL_AUTO_SEQ, NLMSG_NOOP, buffer.len, 0);
	memcpy(data, buffer.buf, buffer.len);

	PyBuffer_Release(&buffer);

	return (PyObject *) message;
}

#define parse_header_docs "Parses the message's header (base NetLink level).\n@return A tuple of [len, type, flags, seq, pid]"

static PyObject *message_parse_header(Message *self,  PyObject *args) {
	struct nlmsghdr* nlh = nlmsg_hdr(self->msg);

	int len, type, flags, seq, pid;

	len = nlh->nlmsg_len;
	type = nlh->nlmsg_type;
	flags = nlh->nlmsg_flags;
	seq = nlh->nlmsg_seq;
	pid = nlh->nlmsg_pid;

	PyObject* result = Py_BuildValue("(iiiii)", len, type, flags, seq, pid);

	return result;
}

static PyObject *Message_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
    Message *self;

    self = (Message *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void Message_dealloc(Message *self) {
    if (self->msg != NULL) {
        nlmsg_free(self->msg);
    }

    Py_TYPE(self)->tp_free((PyObject *)self);
}

/**
 * @param family_id The family id.
 * @param hdrlen Header length.
 * @param flags flags.
 */
static void Message_init(Message *self, PyObject *args, PyObject *kwds) {
    int family_id;
    int hdrlen;
    int flags;
    
    if (PyArg_ParseTuple(args, "iii", &family_id, &hdrlen, &flags)) {
	    self->msg = nlmsg_alloc();

	    if (!self->msg) {
	       PyErr_SetString(PyExc_ConnectionRefusedError, "Can't allocate memory");
	       return;
	    } 

	    if (!nlmsg_put(self->msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, hdrlen, flags)) {
		nlmsg_free(self->msg);
		return;
	    }
    }
}

static PyMemberDef Message_members[] = {
    {"__message", T_OBJECT_EX, offsetof(Message, msg), 0, "The netlink."},
    {NULL} /* Sentinel */
};

static PyMethodDef Message_methods[] = {
    {"reserve", (PyCFunction) message_reserve, METH_VARARGS, reserve_docs},
    {"append", (PyCFunction) message_append, METH_VARARGS, append_docs},
    {"nla_put", (PyCFunction) message_nla_put, METH_VARARGS, nla_put_docs},
    {"get_bytes", (PyCFunction) message_get_bytes, METH_VARARGS, get_bytes_docs}, 
    {"parse_header", (PyCFunction) message_parse_header, METH_VARARGS, parse_header_docs},
    {"from_bytes", (PyCFunction) message_from_bytes, METH_VARARGS | METH_CLASS, from_bytes_docs},
    {NULL} /* Sentinel */
};

PyTypeObject MessageType = {
    PyVarObject_HEAD_INIT(NULL, 0) "netlink.Message", /* tp_name */
    sizeof(Message),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)Message_dealloc,                      /* tp_dealloc */
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
    Message_methods,        /* tp_methods */
    Message_members,        /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)Message_init, /* tp_init */
    0,                      /* tp_alloc */
    Message_new,            /* tp_new */
};
