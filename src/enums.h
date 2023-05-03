#ifndef ENUMS_H
#define ENUMS_H
#include <Python.h>
#include "netlink.h"


typedef struct {
	PyObject_HEAD
} CB_TYPE;

static PyObject *cb_type_getattr(CB_TYPE *self, PyObject *name) {
    if (PyUnicode_CompareWithASCIIString(name, "CB_VALID") == 0) {
        return PyLong_FromLong(NL_CB_VALID);
    } else if (PyUnicode_CompareWithASCIIString(name, "CB_FINISH") == 0) {
        return PyLong_FromLong(NL_CB_FINISH);
    } else if (PyUnicode_CompareWithASCIIString(name, "CB_OVERRUN") == 0) {
        return PyLong_FromLong(NL_CB_OVERRUN);
    } else {
        // Fall back to default behavior for other attribute accesses
        return PyObject_GenericGetAttr(self, name);
    }
}

PyTypeObject CBTypeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "netlink.CB_TYPE",
    .tp_basicsize = sizeof(CB_TYPE),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_getattr = cb_type_getattr
};



typedef struct {
	PyObject_HEAD
} CB_KIND;


PyTypeObject CBKindType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "netlink.CB_KIND",
    .tp_basicsize = sizeof(CB_KIND),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};

void initialize_enums() {
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_VALID", PyLong_FromLong(NL_CB_VALID));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_INVALID", PyLong_FromLong(NL_CB_INVALID));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_FINISH", PyLong_FromLong(NL_CB_FINISH));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_OVERRUN", PyLong_FromLong(NL_CB_OVERRUN));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_SKIPPED", PyLong_FromLong(NL_CB_SKIPPED));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_ACK", PyLong_FromLong(NL_CB_ACK));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_MSG_IN", PyLong_FromLong(NL_CB_MSG_IN));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_MSG_OUT", PyLong_FromLong(NL_CB_MSG_OUT));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_SEQ_CHECK", PyLong_FromLong(NL_CB_SEQ_CHECK));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_SEND_ACK", PyLong_FromLong(NL_CB_SEND_ACK));
	PyDict_SetItemString(CBTypeType.tp_dict, "CB_DUMP_INTR", PyLong_FromLong(NL_CB_DUMP_INTR));

	PyDict_SetItemString(CBKindType.tp_dict, "CB_DEFAULT", PyLong_FromLong(NL_CB_DEFAULT));
	PyDict_SetItemString(CBKindType.tp_dict, "CB_VERBOSE", PyLong_FromLong(NL_CB_VERBOSE));
	PyDict_SetItemString(CBKindType.tp_dict, "CB_DEBUG", PyLong_FromLong(NL_CB_DEBUG));
	PyDict_SetItemString(CBKindType.tp_dict, "CB_CUSTOM", PyLong_FromLong(NL_CB_CUSTOM));
}

#endif
