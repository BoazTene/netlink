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

#include "attribute_policy.h"

static PyObject *AttributePolicy_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
    AttributePolicy *self;

    self = (AttributePolicy *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void AttributePolicy_dealloc(AttributePolicy *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/**
 * Initializes the attribute policy.
 *
 * @param type The type of the attribute.
 * @param minlen The min length of the attribute.
 * @param maxlen The max length of the attribute.
 */
static void AttributePolicy_init(AttributePolicy *self, PyObject *args, PyObject *kwds) {
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

static PyMemberDef AttributePolicy_members[] = {
    {"type", T_INT, offsetof(AttributePolicy, policy.type), 0, "Type of the attribute."},
    {"minlen", T_INT, offsetof(AttributePolicy, policy.minlen), 0, "Minimum length of the attribute."},
    {"maxlen", T_INT, offsetof(AttributePolicy, policy.maxlen), 0, "Maximum length of the attribute."},
    {NULL} /* Sentinel */
};

static PyMethodDef AttributePolicy_methods[] = {
        {NULL} /* Sentinel */
};

PyTypeObject AttributePolicyType = {
    PyVarObject_HEAD_INIT(NULL, 0) "netlink.AttributePolicy", /* tp_name */
    sizeof(AttributePolicy),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)AttributePolicy_dealloc,                      /* tp_dealloc */
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
    AttributePolicy_methods,        /* tp_methods */
    AttributePolicy_members,        /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)AttributePolicy_init, /* tp_init */
    0,                      /* tp_alloc */
    AttributePolicy_new,            /* tp_new */
};
