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

/*******************************************************************************************
*  Clash with pthread\src\sched.h library -->  'pid_t': redefinition; different basic types
*
*  Add following into pyconfig.h 
*  #ifndef IS_WINDOWS *********
*  typedef int pid_t;
*  #endif             
*  
* //https://stackoverflow.com/questions/38860915/lnk2019-error-in-pycaffe-in-debug-mode-for-caffe-for-windows 
*******************************************************************************************/

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define IS_WINDOWS
#endif

// This code makes us always use the release version of Python
// even when in debug mode of this program.
// https://pytools.codeplex.com/discussions/547562
#define HAVE_ROUND
#ifdef _DEBUG
#define RESTORE_DEBUG
#undef _DEBUG
#endif
#include <Python.h>
#ifdef RESTORE_DEBUG
#define _DEBUG
#undef RESTORE_DEBUG
#endif

#include <stdio.h>
#include "dirent.h"
#include "ZenCommon.h"
#include <stdbool.h>
#include "ZenPython.h"
#include "ZenCoreCLR.h"
#include <ctype.h>

PyGILState_STATE _gil_state;

int _pyDefinitionsCnt = 0;
char _python_path[260];

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_start = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_start = PTHREAD_MUTEX_INITIALIZER;
bool _is_started = false;

Node *_current_node;
Node *_completed_node;

//********************** Start python callback implementations ************************/
PyObject *free_object(PyObject *self, PyObject *args)
{
    PyObject *return_value;
    if (!PyArg_ParseTuple(args, "s", &return_value))
    {
        handle_python_error();
    }
    Py_DECREF(return_value);

    return Py_BuildValue("");
}

PyObject *get_result_raw(PyObject *self, PyObject *args)
{
    common_log_1("START get_result_raw %s", "", LOG_DEBUG);
    char *node_id;
    PyObject *return_value = NULL;
    if (!PyArg_ParseTuple(args, "s", &node_id))
    {
        handle_python_error();
    }

    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
    {
        common_log_1("Node '%s' found...", node_id, LOG_DEBUG);
        int len = coreclr_get_result_len(node);
        if (len > -1)
        {
            char *result = (char *)malloc(len * sizeof(char) + 1);
            coreclr_get_result(node, result);

            common_log_1("result for node '%s' retrieved...", node_id, LOG_DEBUG);
            return_value = Py_BuildValue("s", result);

            common_log_1("return value for node '%s' built...", node_id, LOG_DEBUG);

            free(result);
            common_log_1("result for node '%s' freed...", node_id, LOG_DEBUG);
        }
    }
    else
        common_log_1("WARNING: Node with id '%s' does not exists....", node_id, LOG_WARN);

    common_log_1("END get_result_raw %s", "", LOG_DEBUG);

    return return_value;
}

PyObject *set_element_arg(PyObject *self, PyObject *args)
{
    common_log_1("START set_element_arg %s", "...", LOG_DEBUG);
    char *node_id, *property_value;

    if (!PyArg_ParseTuple(args, "ss", &node_id, &property_value))
    {
        handle_python_error();
    }

    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
        common_set_node_param_arg((Node *)node, property_value);
    else
        common_log_1("WARNING: Node with id '%s' does not exists....", node_id, LOG_WARN);

    common_log_1("END set_element_arg %s", "...", LOG_DEBUG);
    return Py_BuildValue("s", "OK");
}

PyObject *get_element_arg(PyObject *self, PyObject *args)
{
    PyObject *return_value = NULL;

    common_log_1("START get_element_arg %s", "...", LOG_DEBUG);
    char *node_id;

    if (!PyArg_ParseTuple(args, "s", &node_id))
    {
        handle_python_error();
    }

    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
        return_value = Py_BuildValue("s", common_get_node_param_arg((Node *)node));
    else
        common_log_1("WARNING: Node with id '%s' does not exists....", node_id, LOG_WARN);

    common_log_1("END get_element_arg %s", "...", LOG_DEBUG);
    return return_value;
}

PyObject *set_element_property(PyObject *self, PyObject *args)
{
    common_log_1("START set_element_property %s", "...", LOG_DEBUG);
    char *node_id, *property_id, *property_value;

    if (!PyArg_ParseTuple(args, "sss", &node_id, &property_id, &property_value))
    {
        handle_python_error();
    }

    Node *node = common_get_node_by_id(node_id);
    common_set_node_arg((Node *)node, property_id, property_value);
    common_log_1("END set_element_property %s", "...", LOG_DEBUG);
    return Py_BuildValue("s", "OK");
}

PyObject *get_element_property(PyObject *self, PyObject *args)
{
    common_log_1("START get_element_property %s", "...", LOG_DEBUG);
    char *node_id, *property_id;

    if (!PyArg_ParseTuple(args, "ss", &node_id, &property_id))
    {
        handle_python_error();
    }

    Node *node = common_get_node_by_id(node_id);

    PyObject *return_value = Py_BuildValue("s", common_get_node_arg((Node *)node, property_id));
    //Py_XDECREF(return_value);
    common_log_1("END get_element_property %s", "...", LOG_DEBUG);
    return return_value;
}

PyObject *exec(PyObject *self, PyObject *args)
{
    common_log_1("START exec %s", "...", LOG_DEBUG);
    char *node_id;

    if (!PyArg_ParseTuple(args, "s", &node_id))
    {
        handle_python_error();
    }

    Node *node = common_get_node_by_id(node_id);
    if (node != NULL)
    {
        if (common_is_loop_blocker_mode() == true && node->ptrTrueChilds != NULL)
        {
            pthread_mutex_lock(&lock);
            common_exec_node(node);
            pthread_cond_wait(&cond, &lock);
            pthread_mutex_unlock(&lock);
        }
        else
            common_exec_node(node);
    }
    else
        common_log_1("WARNING: Node with id '%s' does not exists....", node_id, LOG_WARN);

    common_log_1("END exec %s", "...", LOG_DEBUG);
    return Py_BuildValue("s", "OK");
}
//********************** End python callback implementations **************************/
EXTERN_DLL_EXPORT int onNodeComplete(Node *completedNode, Node *currentNode)
{
    // Signal node that can proceed, because onEngineStarted can occure before executeAction of parent node
    // For example consider following flow:
    // START -> CREATE_PRICES_OBJECT -> GUI
    // GUI can fire up before actual prices object is created
    // Prices object is created in executeAction and GUI is started onEngineStarted, which can occure before executeAction
    // We must signal here GUI node that his parents executeAction has finished
    if (_current_node != NULL && _is_started == false)
    {
        int i;
        for (i = 0; i < completedNode->trueChildsCnt; i++)
        {
            if (strcmp(completedNode->ptrTrueChilds[i]->id, _current_node->id) == 0)
            {
                _is_started = true;
                pthread_mutex_lock(&lock_start);
                pthread_cond_signal(&cond_start);
                pthread_mutex_unlock(&lock_start);
            }
        }
    }
    // Check if currentNode is last node (without childs)
    else if (common_is_loop_blocker_mode() == true && strcmp(_current_node->id, currentNode->id) == 0 && completedNode->ptrTrueChilds == NULL)
    {
        _completed_node = completedNode;
        pthread_mutex_lock(&lock);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
    }
    return 0;
}

EXTERN_DLL_EXPORT int onImplementationLoad(char implementationId[50])
{
    if (!Py_IsInitialized())
    {
        Py_OptimizeFlag = 1;
        Py_UnbufferedStdioFlag = 1;

        Py_SetProgramName(L"ZenPython");

        PyImport_AppendInittab("algonia_helpers", &PyInit_zen_helpers);

        Py_Initialize();

        if (strcmp(COMMON_ENGINE_CONFIGURATION.AddLibsRootToPath, "0") != 0)
        {
            PyObject *sys = PyImport_ImportModule("sys");
            PyObject *path = PyObject_GetAttrString(sys, "path");

            addPyRootPaths("./libs", path);
        }

        if (
            strcmp(COMMON_ENGINE_CONFIGURATION.pythonModulesPath36, "") != 0 && strcmp(implementationId, "ZenPython36") == 0 ||
            strcmp(COMMON_ENGINE_CONFIGURATION.pythonModulesPath37, "") != 0 && strcmp(implementationId, "ZenPython37") == 0 ||
            strcmp(COMMON_ENGINE_CONFIGURATION.pythonModulesPath38, "") != 0 && strcmp(implementationId, "ZenPython38") == 0)
        {
            PyObject *sys = PyImport_ImportModule("sys");
            PyObject *path = PyObject_GetAttrString(sys, "path");

            int numPaths = 0;
            char **paths = NULL;
            if (strcmp(implementationId, "ZenPython36") == 0)
                common_str_split(COMMON_ENGINE_CONFIGURATION.pythonModulesPath36, ";", &numPaths, &paths);
            else if (strcmp(implementationId, "ZenPython37") == 0)
                common_str_split(COMMON_ENGINE_CONFIGURATION.pythonModulesPath37, ";", &numPaths, &paths);
            else if (strcmp(implementationId, "ZenPython38") == 0)
                common_str_split(COMMON_ENGINE_CONFIGURATION.pythonModulesPath38, ";", &numPaths, &paths);

            int i = 0;
            for (i = 0; i < numPaths; i++)
                PyList_Append(path, PyUnicode_FromString(paths[i]));

            common_free_splitted_string(paths, numPaths);
        }

        PyEval_InitThreads();

        if (COMMON_ENGINE_CONFIGURATION.displayDebug == 1)
        {
            printf("\nPython version: %s\n", Py_GetVersion());
            printf("Python home: %ls\n", Py_GetPythonHome());
            printf("Default module search path: %ls\n\n", Py_GetPath());
        }
    }
    return EXIT_SUCCESS;
}

EXTERN_DLL_EXPORT int onNodePreInit(Node *node)
{
    PyObject *pargs, *p_module = NULL, *p_class, *p_instance;

    ensure_GIL();

    switch (atoi(common_get_node_arg(node, "PY_RESULT_TYPE")))
    {
    case 0:
        node->lastResult = malloc(sizeof(int *));
        *node->lastResult = malloc(sizeof(int));
        node->lastResultType = RESULT_TYPE_INT;
        break;

    case 1:
        node->lastResult = NULL;
        node->lastResultType = RESULT_TYPE_CHAR_ARRAY;
        break;
    }

    p_module = createPyModule(node);

    p_class = PyObject_GetAttrString(p_module, common_get_node_arg(node, "PY_CLASS"));
    handle_python_error();

    char *py_args_decoded = common_str_decode(common_get_node_arg(node, "PY_CONSTRUCTOR_ARG"));
    pargs = Py_BuildValue("(s)", py_args_decoded);
    free(py_args_decoded);

    p_instance = PyEval_CallObject(p_class, pargs);
    Py_DECREF(pargs);
    handle_python_error();

    pyDefinitions[_pyDefinitionsCnt].nodeId = (char *)malloc(sizeof(char) * (strlen(node->id) + 1));
    strcpy(pyDefinitions[_pyDefinitionsCnt].nodeId, node->id);

    pyDefinitions[_pyDefinitionsCnt].pyMethod = PyObject_GetAttrString(p_instance, common_get_node_arg(node, "PY_FUNCT"));
    handle_python_error();

    pyDefinitions[_pyDefinitionsCnt].pyOnNodeCompleteMethod = PyObject_GetAttrString(p_instance, "onNodeComplete");
    pyDefinitions[_pyDefinitionsCnt].pyRegisterExecNodesMethod = PyObject_GetAttrString(p_instance, "registerExecNodes");
    pyDefinitions[_pyDefinitionsCnt].pyGetExecNodesMethod = PyObject_GetAttrString(p_instance, "execNodes");

    PyErr_Clear();

    release_GIL();

    node->languageUsed = PY;
    node->implementationContext = malloc(sizeof(int));
    *((int *)(node->implementationContext)) = _pyDefinitionsCnt;

    _pyDefinitionsCnt++;

    return 0;
}

void execute_main_py_method(Node *node)
{
    common_log_1("start execute_main_py_method for node:'%s'", node->id, LOG_DEBUG);

    PyObject *pargs = Py_BuildValue("()");

    common_log_1("executing method for node:'%s'", node->id, LOG_DEBUG);
    PyObject *pres = PyEval_CallObject(pyDefinitions[*((int *)(node->implementationContext))].pyMethod, pargs);

    if (strcmp(common_get_node_arg(node, "DEBUG"), "0") != 0)
    {
        common_log_1("start removing debug file for node:'%s'....", node->id, LOG_DEBUG);
        char py_file[256];
        snprintf(py_file, sizeof(py_file), "%s.py", node->id);
        remove(py_file);
        common_log_1("debug file for node:'%s' removed....", node->id, LOG_DEBUG);
    }

    handle_python_error();

    char *result = NULL;

    switch (atoi(common_get_node_arg(node, "PY_RESULT_TYPE")))
    {
    case 0:
        common_log_1("start parsing int result for node:'%s'", node->id, LOG_DEBUG);
        PyArg_Parse(pres, "i", ((int *)*node->lastResult));
        common_log_1("end parsing int result for node:'%s'", node->id, LOG_DEBUG);
        //printf("%d\n",*((int*)*node->lastResult));
        break;

    case 1:
        common_log_1("start parsing string result for node:'%s'", node->id, LOG_DEBUG);
        PyArg_Parse(pres, "s", &result);
        if (result != NULL)
        {
            if (node->lastResult != NULL)
            {
                common_log_1("freeing result for node:'%s'", node->id, LOG_DEBUG);
                free(*node->lastResult);
                free(node->lastResult);
            }
            common_log_1("allocating result for node:'%s'", node->id, LOG_DEBUG);
            node->lastResult = (char **)(malloc(sizeof(char *)));
            *node->lastResult = (char *)malloc(sizeof(char) * (strlen(result) + 1));
            strcpy(*node->lastResult, result);
            common_log_2("result for node:'%s' is '%s'", node->id, result, LOG_DEBUG);
        }
        else
            common_log_1("result for node:'%s' IS NULL!", node->id, LOG_DEBUG);
        break;
    }

    Py_DECREF(pres);
    common_log_1("end execute_main_py_method for node:'%s'", node->id, LOG_DEBUG);
    //set_disconected_nodes(node);
}

EXTERN_DLL_EXPORT int onEngineStarted(Node *node)
{
    if (strcmp(common_get_node_arg(node, "_LOOP_BLOCKER_"), "0") != 0)
    {
        _completed_node = NULL;
        _current_node = node;

        pthread_mutex_lock(&lock_start);
        pthread_cond_wait(&cond_start, &lock_start);
        pthread_mutex_unlock(&lock_start);

        common_set_is_loop_blocker_mode();
        ensure_GIL();

        execute_main_py_method(node);
    }
    return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
    if (strcmp(common_get_node_arg(node, "_LOOP_BLOCKER_"), "0") == 0)
    {
        common_log_1("ensure_GIL for node:'%s'", node->id, LOG_DEBUG);

        if (!common_is_loop_blocker_mode())
            ensure_GIL();

        execute_main_py_method(node);
        common_log_1("release_GIL for node:'%s'", node->id, LOG_DEBUG);

        if (!common_is_loop_blocker_mode())
            release_GIL();
        node->isConditionMet = 1;
    }
    return 0;
}

EXTERN_DLL_EXPORT Node **getNodesToExecute(Node *node, int *nodesToExecuteCnt)
{
    char *node_ids;
    int numNodesToExecute = 0;
    char **nodesToExecute = NULL;

    ensure_GIL();
    if (!pyDefinitions[*((int *)(node->implementationContext))].pyRegisterExecNodesMethod ||
        !PyCallable_Check(pyDefinitions[*((int *)(node->implementationContext))].pyRegisterExecNodesMethod))
    {
        release_GIL(_gil_state);
        return NULL;
    }

    PyObject *pargs = Py_BuildValue("()");

    PyObject *pres = PyEval_CallObject(pyDefinitions[*((int *)(node->implementationContext))].pyRegisterExecNodesMethod, pargs);

    handle_python_error();

    PyArg_Parse(pres, "s", &node_ids);
    Py_DECREF(pres);
    handle_python_error();
    release_GIL();

    return common_fill_disconnected_nodes(node_ids, nodesToExecuteCnt);
}

void set_disconected_nodes(Node *node)
{
    if (pyDefinitions[*((int *)(node->implementationContext))].pyGetExecNodesMethod &&
        PyCallable_Check(pyDefinitions[*((int *)(node->implementationContext))].pyGetExecNodesMethod))
    {

        char *node_ids;

        PyObject *pargs = Py_BuildValue("()");

        PyObject *pres = PyEval_CallObject(pyDefinitions[*((int *)(node->implementationContext))].pyGetExecNodesMethod, pargs);

        handle_python_error();

        PyArg_Parse(pres, "s", &node_ids);
        Py_DECREF(pres);
        handle_python_error();

        node->disconnectedNodes = common_fill_disconnected_nodes(node_ids, &node->disconnectedNodesCnt);
    }
}

PyObject *PyInit_zen_helpers(void)
{
    return PyModule_Create(&modDef);
}

PyObject *createPyModule(Node *node)
{
    char py_file[256];
    snprintf(py_file, sizeof(py_file), "%s.py", node->id);
    char *py_script_decoded = common_str_decode(common_get_node_arg(node, "PY_SCRIPT_TEXT"));

    if (strcmp(common_get_node_arg(node, "DEBUG"), "0") != 0)
    {
        FILE *fp;
        char py_debug_file[256];
        char debug_pre[] = "#DEBUG#";
        char debug_post[] = "webbrowser.open('http://localhost:5555');basedir = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) ;sys.path.append(basedir);from web_pdb import set_trace; set_trace()";

        py_script_decoded = common_replace_word(py_script_decoded, debug_pre, debug_post);

        snprintf(py_debug_file, sizeof(py_debug_file), "%s/%s", "tmp", py_file);

        fp = fopen(py_file, "w+");
        fprintf(fp, "%s", py_script_decoded);
        fclose(fp);
    }

    PyObject *builtins = PyEval_GetBuiltins();
    handle_python_error();

    PyObject *compile = PyDict_GetItemString(builtins, "compile");
    handle_python_error();

    PyObject *code = PyObject_CallFunction(compile, "sss", py_script_decoded, py_file, "exec");
    free(py_script_decoded);
    handle_python_error();

    PyObject *p_module = PyImport_ExecCodeModule(node->id, code);
    handle_python_error();
    return p_module;
}

void handle_python_error()
{
    if (PyErr_Occurred())
    {
        PyErr_Print();
        fflush(stdout);
        getchar();
        exit(1);
    }
}

void addPyRootPaths(const char *name, PyObject *pyPath)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%s\n", path);
            PyList_Append(pyPath, PyUnicode_FromString(path));
            addPyRootPaths(path, pyPath);
        }
    }
    closedir(dir);
}

void ensure_GIL()
{
    if (PyGILState_Check() == 0)
    {
        common_log_1("ensure_GIL PyGILState_Check(): '%s'", "0", LOG_DEBUG);
        _gil_state = PyGILState_Ensure();
        common_log_1("GIL ensured%s", "...", LOG_DEBUG);
    }
    else
        common_log_1("ensure_GIL PyGILState_Check(): '%s'", "1", LOG_DEBUG);
}

void release_GIL()
{
    if (PyGILState_Check() == 1)
    {
        common_log_1("release_GIL PyGILState_Check(): '%s'", "1", LOG_DEBUG);
        PyGILState_Release(_gil_state);
        common_log_1("GIL released%s", "...", LOG_DEBUG);
    }
    else
        common_log_1("release_GIL PyGILState_Check(): '%s'", "0", LOG_DEBUG);
}
/*
char* parse_script(Node* node)
{
    char* script =    common_get_node_arg(node, "PY_SCRIPT_TEXT");
    const char *regex = "exec(.*?)";
    
    struct slre_cap slre_outer_cap[2];
    struct slre_cap slre_inner_cap[2];
    int i, j = 0, str_len = (int) strlen(script);
    char node_ids [NODE_ID_LENGTH * MAX_EXECUTE_CALLS] = "";
    
    int iCnt=1;
    while ((i = slre_match(regex, script + j, str_len - j, slre_outer_cap, 2, SLRE_IGNORE_CASE)) > 0 &&
        j < str_len) 
    {
        slre_match("&quot;(.*)", slre_outer_cap[0].ptr, strlen(slre_outer_cap[0].ptr), slre_inner_cap, 0, SLRE_IGNORE_CASE);

        common_concatenate_string(node_ids,slre_inner_cap[0].ptr,NODE_ID_LENGTH);
        common_concatenate_string(node_ids,",",1);
        
        iCnt ++;
        j += i;
    }
    return node_ids;
}
*/