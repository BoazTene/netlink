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

/*
 * This file exports enums to the module.
 *
 * The enum would like this in python:
 *
 * class SomeEnum:
 * 	SOME_VALUE = 1
 */

#ifndef ENUMS_H
#define ENUMS_H

#include <Python.h>
#include "netlink.h"
#include "attribute.h"

typedef struct {
	PyObject_HEAD
} CB_TYPE;

PyTypeObject CBTypeType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "netlink.CB_TYPE",
    .tp_basicsize = sizeof(CB_TYPE),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
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

/**
 * Initializes the enums, should be callled on the initializatino of the module.  *
 */
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

	PyDict_SetItemString(AttributeType.tp_dict, "UNSPEC", PyLong_FromLong(NLA_UNSPEC));
	PyDict_SetItemString(AttributeType.tp_dict, "U8", PyLong_FromLong(NLA_U8));
	PyDict_SetItemString(AttributeType.tp_dict, "U16", PyLong_FromLong(NLA_U16));
	PyDict_SetItemString(AttributeType.tp_dict, "U32", PyLong_FromLong(NLA_U32));
	PyDict_SetItemString(AttributeType.tp_dict, "U64", PyLong_FromLong(NLA_U64));
	PyDict_SetItemString(AttributeType.tp_dict, "STRING", PyLong_FromLong(NLA_STRING));
	PyDict_SetItemString(AttributeType.tp_dict, "FLAG", PyLong_FromLong(NLA_FLAG));
	PyDict_SetItemString(AttributeType.tp_dict, "MSECS", PyLong_FromLong(NLA_MSECS));
	PyDict_SetItemString(AttributeType.tp_dict, "NESTED", PyLong_FromLong(NLA_NESTED));
}

#endif
