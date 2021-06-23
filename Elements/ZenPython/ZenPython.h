/*************************************************************************
 * Copyright (c) 2020, 2021 Algonia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contributors:
 *    Tomaž Vinko
 *   
 **************************************************************************/
#define MAX_EXECUTE_CALLS 200

void execute_main_py_method(Node *node);
void addPyRootPaths(const char *name, PyObject *pyPath);
void handle_python_error();
void set_disconected_nodes(Node *node);
char *parse_script(Node *node);
void release_GIL();
void ensure_GIL();
PyObject *PyInit_zen_helpers(void);
PyObject *get_result_raw(PyObject *self, PyObject *args);
PyObject *set_element_property(PyObject *self, PyObject *args);
PyObject *get_element_property(PyObject *self, PyObject *args);
PyObject *free_object(PyObject *self, PyObject *args);
PyObject *set_element_arg(PyObject *self, PyObject *args);
PyObject *get_element_arg(PyObject *self, PyObject *args);
PyObject *exec(PyObject *self, PyObject *args);
PyObject *createPyModule(Node *node);

struct pyDefinition
{
    char *nodeId;
    PyObject *pyMethod;
    PyObject *pyRegisterExecNodesMethod;
    PyObject *pyGetExecNodesMethod;
    PyObject *pyOnNodeCompleteMethod;
};
struct pyDefinition pyDefinitions[1000];

struct PyMethodDef methods[] = {
    {"get_result_raw", get_result_raw, METH_VARARGS, "Returns element result"},
    {"set_element_property", set_element_property, METH_VARARGS, "Sets element property "},
    {"set_element_arg", set_element_arg, METH_VARARGS, "Sets element argument "},
    {"get_element_arg", get_element_arg, METH_VARARGS, "Gets element argument "},
    {"get_element_property", get_element_property, METH_VARARGS, "Returns element property"},
    {"free_object", free_object, METH_VARARGS, "Freeing object from unmanaged memory"},
    {"exec", exec, METH_VARARGS, "Executes element"},
    {NULL, NULL, 0, NULL}};

struct PyModuleDef modDef = {
    PyModuleDef_HEAD_INIT, "algonia_helpers", NULL, -1, methods,
    NULL, NULL, NULL, NULL};
