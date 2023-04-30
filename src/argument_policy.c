#include "argument_policy.h"

static PyObject *Argument_Policy_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
    Argument_Policy *self;

    self = (Argument_Policy *)type->tp_alloc(type, 0);

    self->policy = NULL;

    return (PyObject *)self;
}

static void Argument_Policy_dealloc(Argument_Policy *self) {
    if (self->policy != NULL) {
        free(self->policy);
    }

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static void Argument_Policy_init(Argument_Policy *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"type", "minlen", "maxlen"}
    int type;
    int minlen;
    int maxlen;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iii", &type, &minlen, &maxlen)) return;

    self->policy = {
        .type = type,
        .minlen = minlen,
        .maxlen = maxlen,
    };
}

static PyMemberDef Argument_Policy_members[] = {
    {"type", T_INT, offsetof(Argument_Policy, policy->type), 0, "Type of the argument."},
    {"minlen", T_INT, offsetof(Argument_Policy, policy->minlen), 0, "Minimum length of the argument."},
    {"maxlen", T_INT, offsetof(Argument_Policy, policy->maxlen), 0, "Maximum length of the argument."},
    {NULL} /* Sentinel */
};

static PyMethodDef Argument_Policy_methods[] = {
    {"reserve", message_reserve, METH_VARARGS, reserve_docs},
    {"append", message_append, METH_VARARGS, append_docs},
    {"nla_put", message_nla_put, METH_VARARGS, nla_put_docs},
    {NULL} /* Sentinel */
};

PyTypeObject Argument_PolicyType = {
    PyVarObject_HEAD_INIT(NULL, 0) "netlink.Argument_Policy", /* tp_name */
    sizeof(Argument_Policy),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)Argument_Policy_dealloc,                      /* tp_dealloc */
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
    Argument_Policy_methods,        /* tp_methods */
    Argument_Policy_members,        /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)Argument_Policy_init, /* tp_init */
    0,                      /* tp_alloc */
    Argument_Policy_new,            /* tp_new */
};
