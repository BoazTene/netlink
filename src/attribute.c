#include "attribute.h"

static PyObject *Attribute_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwds) {
    Attribute *self;

    self = (Attribute *)type->tp_alloc(type, 0);

    return (PyObject *)self;
}

static void Attribute_dealloc(Attribute *self) {
	if (self->data != NULL) {
		free(self->data);
	}
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static void Attribute_init(Attribute *self, PyObject *args, PyObject *kwds) {
    Py_buffer data;
    int len;
    int type;
    if (!PyArg_ParseTuple(args, "y*ii", &data, &len, &type)) return;

    self->data = (unsigned char *) malloc(data.len);
    memcpy(self->data, data.buf, data.len);
    self->len = len;
    self->type = type;

    PyBuffer_Release(&data);
    return;
}

static PyMemberDef Attribute_members[] = {
    {"len", T_INT, offsetof(Attribute, len), 0, "The netlink."},
    {"type", T_INT, offsetof(Attribute, type), 0, "The netlink."},
    {"data", T_STRING, offsetof(Attribute, data), 0, "The netlink."},
    {NULL} /* Sentinel */
};

static PyMethodDef Attribute_methods[] = {
       {NULL} /* Sentinel */
};

PyTypeObject AttributeType = {
    PyVarObject_HEAD_INIT(NULL, 0) "netlink.Attribute", /* tp_name */
    sizeof(Attribute),                                  /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)Attribute_dealloc,                      /* tp_dealloc */
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
    Attribute_methods,        /* tp_methods */
    Attribute_members,        /* tp_members */
    0,                      /* tp_getset */
    0,                      /* tp_base */
    0,                      /* tp_dict */
    0,                      /* tp_descr_get */
    0,                      /* tp_descr_set */
    0,                      /* tp_dictoffset */
    (initproc)Attribute_init, /* tp_init */
    0,                      /* tp_alloc */
    Attribute_new,            /* tp_new */
};
