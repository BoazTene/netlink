#include "argument_policy.h"

static PyObject *ArgumentPolicy_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
    ArgumentPolicy *self;

    self = (ArgumentPolicy *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void ArgumentPolicy_dealloc(ArgumentPolicy *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static void ArgumentPolicy_init(ArgumentPolicy *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"type", "minlen", "maxlen"};
    int type;
    int minlen;
    int maxlen;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iii", kwlist, &type, &minlen, &maxlen)) return;

    self->policy = (struct nla_policy) {
        .type = type,
        .minlen = minlen,
        .maxlen = maxlen,
    };
}

static PyMemberDef ArgumentPolicy_members[] = {
    {"type", T_INT, offsetof(ArgumentPolicy, policy.type), 0, "Type of the argument."},
    {"minlen", T_INT, offsetof(ArgumentPolicy, policy.minlen), 0, "Minimum length of the argument."},
    {"maxlen", T_INT, offsetof(ArgumentPolicy, policy.maxlen), 0, "Maximum length of the argument."},
    {NULL} /* Sentinel */
};

static PyMethodDef ArgumentPolicy_methods[] = {
        {NULL} /* Sentinel */
};

PyTypeObject ArgumentPolicyType = {
    PyVarObject_HEAD_INIT(NULL, 0) "netlink.ArgumentPolicy", /* tp_name */
    sizeof(ArgumentPolicy),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)ArgumentPolicy_dealloc,                      /* tp_dealloc */
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
    ArgumentPolicy_methods,        /* tp_methods */
    ArgumentPolicy_members,        /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)ArgumentPolicy_init, /* tp_init */
    0,                      /* tp_alloc */
    ArgumentPolicy_new,            /* tp_new */
};
