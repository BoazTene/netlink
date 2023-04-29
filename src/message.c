#include "message.h"


static const char* reserve_docs = "Reserves room for additional data at the tail of the an existing netlink message. Eventual padding required will be zeroed out."
                                  "@param len length of additional data to reserve room for"
                                  "@param pad number of bytes to align data to"
                                  "@return null";
static PyObject * message_reserve(Message *self, PyObject *args) {
    int len;
    int pad;

    if (!Py_ParseTuple(args, "ii", &len, &pad)) {
        return NULL;
    }

    nlmsg_reserve(self->msg, len, pad);

    PY_RETURN_NONE;
}

static const char *append_docs = "Append data to tail of a netlink message"
                                 "@param data data to add"
                                 "@param len length of data"
                                 "@param pad Number of bytes to align data to";
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

    PY_RETURN_NONE; 
}

static const char *nla_put_docs = "Add a unspecific attribute to netlink message."
                                 "@param attribute_type Attribute type."
                                 "@param data Pointer to data to be used as attribute payload.";
static PyObject * message_nla_put(Message *self, PyObject *args) {
    Py_buffer buffer;
    int attribute_type;
    
    if (!PyArg_ParseTuple(args, "y*i", &buffer, &attribute_type)) {
            return NULL;
    }

    nla_put(self->msg, attribute_type, buffer.len, (void *) buffer.buf);

    PyBuffer_Release(&buffer);

    PY_RETURN_NONE;
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

static void Message_init(Message *self, PyObject *args, PyObject *kwds) {
    int family_id;
    int flags;
    int hdrlen;
    int cmd;
    int version;

    if (!PyArg_ParseTuple(args, "iiiii", &family_id, &hdrlen, &flags, &cmd, &version)) return;

    self->msg = nlmsg_alloc();

    if (!self->msg) {
       PyErr_SetString(PyExc_ConnectionRefusedError, "Can't allocate memory");
       return;
    } 

    if (!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, hdrlen, flags, cmd, version)) {
        nlmsg_free(self->msg);
        return;
    }
}

static PyMemberDef Message_members[] = {
    {"__message", T_OBJECT_EX, offsetof(Message, msg), 0, "The netlink."},
    {NULL} /* Sentinel */
};

static PyMethodDef Message_methods[] = {
    {"reserve", message_reserve, METH_VARARGS, reserve_docs},
    {"append", message_append, METH_VARARGS, append_docs},
    {"nla_put", message_nla_put, METH_VARARGS, nla_put_docs},
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
    Py_TPFLAGS_DEFAULT,                               /* tp_flags */
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
