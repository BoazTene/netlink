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
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "netlink_class.h"

static struct PyModuleDef netlink = {
    PyModuleDef_HEAD_INIT, "netlink", /* name of module */
    "Fast and simple implementation of netlink client.\nWritting using "
    "c-api.\n@author Boaz Tene", /* module documentation, may be NULL */
    -1,                          /* size of per-interpreter state of the module,
                                    or -1 if the module keeps state in global variables. */
    NULL};

PyMODINIT_FUNC PyInit_netlink(void) {
  PyObject *module;

  if (PyType_Ready(&NetLinkType) < 0)
    return NULL;

  module = PyModule_Create(&netlink);

  if (!module) {
    return NULL;
  }

  Py_INCREF(&NetLinkType);
  PyModule_AddObject(module, "NetLink", (PyObject *)&NetLinkType);
  return module;
}

int main(int argc, char *argv[]) {
  wchar_t *program = Py_DecodeLocale(argv[0], NULL);
  if (program == NULL) {
    fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
    exit(1);
  }

  /* Add a built-in module, before Py_Initialize */
  if (PyImport_AppendInittab("netlink", PyInit_netlink) == -1) {
    fprintf(stderr, "Error: could not extend in-built modules table\n");
    exit(1);
  }

  /* Pass argv[0] to the Python interpreter */
  Py_SetProgramName(program);

  /* Initialize the Python interpreter.  Required.
     If this step fails, it will be a fatal error. */
  Py_Initialize();

  /* Optionally import the module; alternatively,
     import can be deferred until the embedded script
     imports it. */
  PyObject *pmodule = PyImport_ImportModule("netlink");
  if (!pmodule) {
    PyErr_Print();
    fprintf(stderr, "Error: could not import module 'spam'\n");
  }

  PyMem_RawFree(program);
  return 0;
}
