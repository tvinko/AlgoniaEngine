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

/*=========================================================================
|
|       .NET core binding for Native Zeno Engine
|
|         * Each managed Element needs native wrapper that act like bridge 
|           to native Zeno Engine. It translates requests from managed 
|			Elements to Engine and requests from Engine to managed Elements:
|         		* Requests from managed Elements are : getting values of
|				  Element properies, Element results and Element result infos
|         		* Requests from native Zeno Engine are : execute action, 
|				  init managed Elements, getting dynamic Elements....
|
+---------------------------------------------------------------------------
|
|   Known Bugs:  * None
|	     To Do:  * None
|
*============================================================================*/

#include <iostream>
#include "ZenCommon.h"
#include "os_call.h"
#if defined(_WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd
#include <stdio.h>
#include "mscoree.h"
#include "ZenCoreCLR.h"
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <set>
#include <dirent.h>
#include <sys/stat.h>
#include "coreclrhost.h"
#include "ZenCoreCLR.h"

using namespace std;
coreclr_initialize_ptr coreclr_init;
coreclr_create_delegate_ptr coreclr_create_dele;
#endif

const char *coreClrDir;
std::string netCoreDir;

unsigned long _domainId = 0;
int _areNodeDatasInitialized = 0;

// Node data struct that is passed to managed side
struct nodeData
{
    char id[50];
    void *ptr;
};
struct nodeData nodeDatas[1000];

//Assembly data
struct assemblyData
{
    char id[50];
    void *InitUnmanagedElementsFp;
    void *ExecuteActionFp;
    void *OnElementInitFp;
    void *GetDynamicElementsFp;
    void *GetResultFp;
    void *GetResultLenFp;
};
struct assemblyData assemblyDatas[1000];
int _assembliesCnt = -1;

#if defined(_WIN32)
HMODULE _coreCLRModule;
#endif

//Probe locations
#if defined(_WIN32)
int tpaSize = 100 * MAX_PATH;
wchar_t appPaths[MAX_PATH * 50];
wchar_t appNiPaths[MAX_PATH * 50];
wchar_t nativeDllSearchDirectories[MAX_PATH * 50];
wchar_t platformResourceRoots[MAX_PATH * 50];
wchar_t *trustedPlatformAssemblies = new wchar_t[tpaSize];
#endif

#if defined(_WIN32)
ICLRRuntimeHost2 *_runtimeHost;
#else
#define __stdcall __attribute__((stdcall))
void *coreclr_handle;
void *coreclr;
#endif

// Callbacks:
//	* node related : get node property, result, result info
typedef void (*AddEventToBufferCallback)(void *, char *);
typedef char *(*GetElementPropertyCallback)(void *, char *);
typedef void (*SetElementPropertyCallback)(void *, char *, char *);
typedef int (*GetElementResultInfoCallback)(void *);
typedef void **(*GetElementResultCallback)(void *);
typedef void (*ExecuteElementCallback)(void *);

// Pointer to event handler in node implementation
typedef int (*ptrOnNodeEvent)(Node *, char *);

// Pointer to managed function that returns result back to engine
typedef void (*ptrOnGetNodeResult)(char *, char *);

// Node workflow functions
typedef void(InitUnmanagedElementsMethodFp)(char *currentNodeId, nodeData nodes[], int nodesCnt, int isManaged, char *projectRoot, char *projectId, GetElementPropertyCallback getElementPropertyFp, GetElementResultInfoCallback getElementResultInfoFp, GetElementResultCallback getElementResultFp, ExecuteElementCallback executeElementFp, SetElementPropertyCallback setElementPropertyFp, AddEventToBufferCallback addEventToBufferFp);
typedef void(OnElementInitMethodFp)(char *currentNodeId, nodeData nodes[], int nodesCnt, char *result);
typedef void(ExecuteActionMethodFp)(char *currentNodeId, nodeData nodes[], int nodesCnt, char *result);
typedef char *(GetDynamicElementsMethodFp)(char *currentNodeId, nodeData nodes[], int nodesCnt, int isManaged, char *projectRoot, char *projectId, GetElementPropertyCallback getElementPropertyFp, GetElementResultInfoCallback getElementResultInfoFp, GetElementResultCallback getElementResultFp, ExecuteElementCallback executeElementFp, SetElementPropertyCallback setElementPropertyFp, AddEventToBufferCallback addEventToBufferFp);
typedef void(GetResultMethodFp)(char *currentNodeId, char *result);
typedef int(GetResultLenMethodFp)(char *currentNodeId);

//********************** Start callback implementations ************************/
//************************* Calls from managed code **************************/

/**
* Handles event from managed side
*
* @param node		node which fire event
* @param data		event data
* @return           void
*/
void managed_callback_add_event_to_buffer(void *node, char *data)
{
    ptrOnNodeEvent onNodeEventFunct = (ptrOnNodeEvent)GetFunction(((Node *)node)->implementation, "onNodeEvent");
    if (onNodeEventFunct)
        onNodeEventFunct((Node *)node, data);
}

/**
* Handles set node property request from managed code
*
* @param node		node to set property value
* @param key		property key
* @param value		property value
* @return           void
*/
void managed_callback_set_node_property(void *node, char *key, char *value)
{
    common_set_node_arg((Node *)node, key, value);
}

/**
* Returns node property value back to managed code
*
* @param node		node to retrieve property value
* @param key		property key
* @return           node result info
*/
char *managed_callback_get_node_property(void *node, char *key)
{
    return common_get_node_arg((Node *)node, key);
}

/**
* Returns node result info back to managed code
*
* @param node		node to retrieve result
* @return           node result info
*/
int managed_callback_get_node_result_info(void *node)
{
    return ((Node *)node)->lastResultType;
}

/**
* Returns node result back to managed code
*
* @param node		node to retrieve result
* @return           node result
*/
void **managed_callback_get_node_result(void *node)
{
    return ((Node *)node)->lastResult;
}

/**
* Handle node execution request from managed code
*
* @param node		node that is going to be executed
* @return           void
*/
void managed_callback_execute_node(void *node)
{
    common_exec_node((Node *)node);
}
//************************ End callback implementations ************************/

#if defined(_WIN32)
//************************ Start loading CoreClr ******************************/

/**
* Adds trusted assemblies
*
* @return	void
*/
void AddTrustedAssemblies()
{
    trustedPlatformAssemblies[0] = L'\0';

    // Extensions to probe for when finding TPA list files
    char *tpaExtensions[] = {
        "*.dll",
        "*.exe",
        "*.winmd"};

    for (int i = 0; i < _countof(tpaExtensions); i++)
    {
        // Construct the file name search pattern
        char searchPath[MAX_PATH];
        snprintf(searchPath, sizeof(searchPath), "%s%s%s", coreClrDir, "\\", tpaExtensions[i]);

        wchar_t wtext[1000];
        mbstowcs(wtext, searchPath, strlen(searchPath) + 1);

        wchar_t coreCLRRootWide[1000];
        mbstowcs(coreCLRRootWide, coreClrDir, strlen(coreClrDir) + 1);

        // Find files matching the search pattern
        WIN32_FIND_DATAW findData;
        HANDLE fileHandle = FindFirstFileW(wtext, &findData);

        if (fileHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                // Construct the full path of the trusted assembly
                wchar_t pathToAdd[MAX_PATH];
                wcscpy_s(pathToAdd, MAX_PATH, coreCLRRootWide);
                wcscat_s(pathToAdd, MAX_PATH, L"\\");
                wcscat_s(pathToAdd, MAX_PATH, findData.cFileName);

                // Check to see if TPA list needs expanded
                if (wcslen(pathToAdd) + (3) + wcslen(trustedPlatformAssemblies) >= tpaSize)
                {
                    // Expand, if needed
                    tpaSize *= 2;
                    wchar_t *newTPAList = new wchar_t[tpaSize];
                    wcscpy_s(newTPAList, tpaSize, trustedPlatformAssemblies);
                    trustedPlatformAssemblies = newTPAList;
                }

                // Add the assembly to the list and delimited with a semi-colon
                wcscat_s(trustedPlatformAssemblies, tpaSize, pathToAdd);
                wcscat_s(trustedPlatformAssemblies, tpaSize, L";");

            } while (FindNextFileW(fileHandle, &findData));
            FindClose(fileHandle);
        }
    }
}

/**
* Starting the runtime will initialize the JIT, GC, loader, etc.
*
* @return	void
*/
void StartClrRuntime()
{
    wchar_t wtext[1000];
    char coreCLRFile[MAX_PATH];
    char buffCurrDir[FILENAME_MAX];

    GetCurrentDir(buffCurrDir, FILENAME_MAX);
    std::string current_working_dir(buffCurrDir);
    netCoreDir = current_working_dir + "\\NETCoreRuntime";

    coreClrDir = common_directory_exists((netCoreDir).c_str()) ? (netCoreDir).c_str()
                                                               : COMMON_ENGINE_CONFIGURATION.netCorePath;

    snprintf(coreCLRFile, sizeof(coreCLRFile), "%s%s", coreClrDir, "\\coreclr.dll");

    mbstowcs(wtext, coreCLRFile, strlen(coreCLRFile) + 1);
    _coreCLRModule = LoadLibraryExW(wtext, NULL, 0);

    FnGetCLRRuntimeHost pfnGetCLRRuntimeHost = (FnGetCLRRuntimeHost)::GetProcAddress(_coreCLRModule, "GetCLRRuntimeHost");

    if (!pfnGetCLRRuntimeHost)
    {
        printf("ERROR - GetCLRRuntimeHost not found");
        exit(1);
    }

    // Get the hosting interface
    HRESULT hr = pfnGetCLRRuntimeHost(IID_ICLRRuntimeHost2, (IUnknown **)&_runtimeHost);

    if (FAILED(hr))
    {
        printf("ERROR - Failed to get ICLRRuntimeHost2 instance.\nError code:%x\n", hr);
        exit(1);
    }

    hr = _runtimeHost->SetStartupFlags(
        static_cast<STARTUP_FLAGS>(
            STARTUP_FLAGS::STARTUP_LOADER_OPTIMIZATION_MULTI_DOMAIN |
            STARTUP_FLAGS::STARTUP_LOADER_OPTIMIZATION_MULTI_DOMAIN_HOST | // Domain-neutral loading for strongly-named assemblies
            STARTUP_FLAGS::STARTUP_CONCURRENT_GC));

    if (FAILED(hr))
    {
        printf("ERROR - Failed to set startup flags.\nError code:%x\n", hr);
        exit(1);
    }

    // Starting the runtime will initialize the JIT, GC, loader, etc.
    hr = _runtimeHost->Start();
    if (FAILED(hr))
    {
        printf("ERROR - Failed to start the runtime.\nError code:%x\n", hr);
        exit(1);
    }
}

/**
* Add paths to probe in for assemblies which are not one of the well-known Framework assemblies included in the TPA list.
* Currently this is directory where the target application is in.

* @return	void
*/
void SetProbePaths()
{
    char app_path[256];
    snprintf(app_path, sizeof(app_path), "%s%s", COMMON_PROJECT_ROOT, "/Implementations");

    wchar_t targetAppPathWide[MAX_PATH];
    mbstowcs(targetAppPathWide, app_path, strlen(app_path) + 1);

    wchar_t coreRootWide[MAX_PATH];
    mbstowcs(coreRootWide, coreClrDir, strlen(coreClrDir) + 1);

    // APP_PATHS
    // App paths are directories to probe in for assemblies which are not one of the well-known Framework assemblies
    // included in the TPA list.
    //
    // For this simple sample, we just include the directory the target application is in.
    // More complex hosts may want to also check the current working directory or other
    // locations known to contain application assets.

    // Just use the targetApp provided by the user and remove the file name
    wcscpy_s(appPaths, MAX_PATH, targetAppPathWide);

    // APP_NI_PATHS
    // App (NI) paths are the paths that will be probed for native images not found on the TPA list.
    // It will typically be similar to the app paths.
    // For this sample, we probe next to the app and in a hypothetical directory of the same name with 'NI' suffixed to the end.

    wcscpy_s(appNiPaths, targetAppPathWide);
    wcscat_s(appNiPaths, MAX_PATH * 50, L";");
    wcscat_s(appNiPaths, MAX_PATH * 50, targetAppPathWide);
    wcscat_s(appNiPaths, MAX_PATH * 50, L"NI");

    // NATIVE_DLL_SEARCH_DIRECTORIES
    // Native dll search directories are paths that the runtime will probe for native DLLs called via PInvoke

    wcscpy_s(nativeDllSearchDirectories, appPaths);
    wcscat_s(nativeDllSearchDirectories, MAX_PATH * 50, L";");
    wcscat_s(nativeDllSearchDirectories, MAX_PATH * 50, coreRootWide);

    // PLATFORM_RESOURCE_ROOTS
    // Platform resource roots are paths to probe in for resource assemblies (in culture-specific sub-directories)

    wcscpy_s(platformResourceRoots, appPaths);
}

/**
* Creates app domain

* @param	domainName	Firendly domain name
* @return	domain id
*/
unsigned long CreateAppDomain(LPCWSTR domainName)
{
    unsigned long currentDomainId = 0;

    // Setup key/value pairs for AppDomain  properties
    const wchar_t *propertyKeys[] = {
        L"TRUSTED_PLATFORM_ASSEMBLIES",
        L"APP_PATHS",
        L"APP_NI_PATHS",
        L"NATIVE_DLL_SEARCH_DIRECTORIES",
        L"PLATFORM_RESOURCE_ROOTS",
        L"AppDomainCompatSwitch"};

    // Property values which were constructed in step 5
    const wchar_t *propertyValues[] = {
        trustedPlatformAssemblies,
        appPaths,
        appNiPaths,
        nativeDllSearchDirectories,
        platformResourceRoots,
        L"UseLatestBehaviorWhenTFMNotSpecified"};

    int appDomainFlags =
        APPDOMAIN_SECURITY_SANDBOXED |                    // Causes assemblies not from the TPA list to be loaded as partially trusted
        APPDOMAIN_ENABLE_PLATFORM_SPECIFIC_APPS |         // Enable platform-specific assemblies to run
        APPDOMAIN_ENABLE_PINVOKE_AND_CLASSIC_COMINTEROP | // Allow PInvoking from non-TPA assemblies
        APPDOMAIN_DISABLE_TRANSPARENCY_ENFORCEMENT |      // Entirely disables transparency checks
        APPDOMAIN_IGNORE_UNHANDLED_EXCEPTIONS;

    // Create the AppDomain
    HRESULT hr = _runtimeHost->CreateAppDomainWithManager(
        domainName, // Friendly AD name
        appDomainFlags,
        NULL, // Optional AppDomain manager assembly name
        NULL, // Optional AppDomain manager type (including namespace)
        sizeof(propertyKeys) / sizeof(wchar_t *),
        propertyKeys,
        propertyValues,
        &currentDomainId);

    if (FAILED(hr))
    {
        printf("ERROR - Failed to create AppDomain.\nError code:%x\n", hr);
        exit(1);
    }
    return currentDomainId;
}
#endif

/**
* Creates delegates in managed code, that are then called as part of workflow execution
* Namespace and class are constructed from fileName argument. So namespace and class name must be same.
*
* @param fileName					assembly file name
* @param canContainDynamicNodes		if true, it creates also function that node executers use
* @return							void
*/
EXTERN_DLL_EXPORT int coreclr_create_delegates(char *fileName, char *className, contains_dynamic_elements can_contain_dyn_elements)
{
    _assembliesCnt++;

    char assemblyName[MAX_PATH];
    snprintf(assemblyName, sizeof(assemblyName), "%s%s", fileName, ", Version=1.0.0.0, Culture=neutral");

#if defined(_WIN32)
    HRESULT hr;

    wchar_t assemblyNameWide[MAX_PATH];
    mbstowcs(assemblyNameWide, assemblyName, strlen(assemblyName) + 1);

    wchar_t classNameWide[MAX_PATH];
    mbstowcs(classNameWide, className, strlen(className) + 1);

    hr = _runtimeHost->CreateDelegate(_domainId, assemblyNameWide, classNameWide, L"InitUnmanagedElements", (INT_PTR *)&assemblyDatas[_assembliesCnt].InitUnmanagedElementsFp);
    if (FAILED(hr))
    {
        printf("ERROR - Failed to create InitUnmanagedElements.\nError code:%x\n", hr);
        exit(1);
    }

    hr = _runtimeHost->CreateDelegate(_domainId, assemblyNameWide, classNameWide, L"ExecuteAction", (INT_PTR *)&assemblyDatas[_assembliesCnt].ExecuteActionFp);
    hr = _runtimeHost->CreateDelegate(_domainId, assemblyNameWide, classNameWide, L"OnElementInit", (INT_PTR *)&assemblyDatas[_assembliesCnt].OnElementInitFp);
    hr = _runtimeHost->CreateDelegate(_domainId, assemblyNameWide, classNameWide, L"GetResult", (INT_PTR *)&assemblyDatas[_assembliesCnt].GetResultFp);
    hr = _runtimeHost->CreateDelegate(_domainId, assemblyNameWide, classNameWide, L"GetResultLen", (INT_PTR *)&assemblyDatas[_assembliesCnt].GetResultLenFp);

    if (can_contain_dyn_elements == CONTAIN_DYN_ELEMENTS)
    {
        hr = _runtimeHost->CreateDelegate(_domainId, assemblyNameWide, classNameWide, L"GetDynamicElements", (INT_PTR *)&assemblyDatas[_assembliesCnt].GetDynamicElementsFp);
        if (FAILED(hr))
        {
            printf("ERROR - Failed to create GetDynamicElements.\nError code:%x\n", hr);
            exit(1);
        }
    }
#else
    int ret = coreclr_create_dele(
        coreclr_handle,
        _domainId,
        fileName,
        className,
        "InitUnmanagedElements",
        reinterpret_cast<void **>(&assemblyDatas[_assembliesCnt].InitUnmanagedElementsFp));

    if (ret < 0)
    {
        cerr << "couldn't create delegate. err = " << ret << endl;
        exit(1);
    }

    ret = coreclr_create_dele(
        coreclr_handle,
        _domainId,
        fileName,
        className,
        "ExecuteAction",
        reinterpret_cast<void **>(&assemblyDatas[_assembliesCnt].ExecuteActionFp));

    ret = coreclr_create_dele(
        coreclr_handle,
        _domainId,
        fileName,
        className,
        "OnElementInit",
        reinterpret_cast<void **>(&assemblyDatas[_assembliesCnt].OnElementInitFp));

    ret = coreclr_create_dele(
        coreclr_handle,
        _domainId,
        fileName,
        className,
        "GetResult",
        reinterpret_cast<void **>(&assemblyDatas[_assembliesCnt].GetResultFp));

    ret = coreclr_create_dele(
        coreclr_handle,
        _domainId,
        fileName,
        className,
        "GetResultLen",
        reinterpret_cast<void **>(&assemblyDatas[_assembliesCnt].GetResultLenFp));

    /*
	if (ret < 0)
	{
		cerr << "couldn't create delegate. err = " << ret << endl;
		exit(1);
	}*/

    if (can_contain_dyn_elements == CONTAIN_DYN_ELEMENTS)
    {
        ret = coreclr_create_dele(
            coreclr_handle,
            _domainId,
            fileName,
            className,
            "GetDynamicElements",
            reinterpret_cast<void **>(&assemblyDatas[_assembliesCnt].GetDynamicElementsFp));

        if (ret < 0)
        {
            cerr << "couldn't create delegate. err = " << ret << endl;
            exit(1);
        }
    }
#endif
    strncpy(assemblyDatas[_assembliesCnt].id, fileName, strlen(fileName) + 1);
    return _assembliesCnt;
}
//************************** End loading CoreClr ******************************/

//*************** Start exported wrappers to  element workflow functions **************/
EXTERN_DLL_EXPORT void coreclr_init_managed_nodes(int pos, Node *node, is_element_managed is_managed)
{
    InitNodeDatas();
    ((InitUnmanagedElementsMethodFp *)assemblyDatas[pos].InitUnmanagedElementsFp)(node->id, nodeDatas, COMMON_NODE_LIST_LENGTH, is_managed, COMMON_PROJECT_ROOT, COMMON_PROJECT_ID, managed_callback_get_node_property, managed_callback_get_node_result_info, managed_callback_get_node_result, managed_callback_execute_node, managed_callback_set_node_property, managed_callback_add_event_to_buffer);
}

EXTERN_DLL_EXPORT void coreclr_on_node_init(int pos, Node *node, char *result)
{
    ((OnElementInitMethodFp *)assemblyDatas[pos].OnElementInitFp)(node->id, nodeDatas, COMMON_NODE_LIST_LENGTH, result);
}

EXTERN_DLL_EXPORT void coreclr_execute_action(int pos, Node *node, char *result)
{
    ((ExecuteActionMethodFp *)assemblyDatas[pos].ExecuteActionFp)(node->id, nodeDatas, COMMON_NODE_LIST_LENGTH, result);
    if (result != NULL)
        common_log_1("coreclr_execute_action result:'%s'", result, LOG_DEBUG);
}

EXTERN_DLL_EXPORT void coreclr_get_dynamic_nodes(int pos, Node *node, char **result, is_element_managed is_managed)
{
    InitNodeDatas();
    *result = ((GetDynamicElementsMethodFp *)assemblyDatas[pos].GetDynamicElementsFp)(node->id, nodeDatas, COMMON_NODE_LIST_LENGTH, is_managed, COMMON_PROJECT_ROOT, COMMON_PROJECT_ID, managed_callback_get_node_property, managed_callback_get_node_result_info, managed_callback_get_node_result, managed_callback_execute_node, managed_callback_set_node_property, managed_callback_add_event_to_buffer);
}

EXTERN_DLL_EXPORT int coreclr_get_result_len(Node *node)
{
    int pos = *((int *)(node->implementationContext));
    int len;
    len = -1;
    switch (node->languageUsed)
    {
    case CPP:
    case PY:
        if (node->lastResult != NULL)
            len = strlen(*(char **)node->lastResult);
        else
            common_log_1("Node '%s' result IS NULL!", node->id, LOG_WARN);
        break;

    case CS:
        if (assemblyDatas[pos].GetResultLenFp != NULL)
            len = ((GetResultLenMethodFp *)assemblyDatas[pos].GetResultLenFp)(node->id);

        break;
    }

    if (len <= 0)
        common_log_1("Warning: len is zero or less%s", "...", LOG_WARN);

    return len;
}

EXTERN_DLL_EXPORT void coreclr_get_result(Node *node, char *result)
{
    int pos = *((int *)(node->implementationContext));
    switch (node->languageUsed)
    {
    case CPP:
    case PY:
        if (node->lastResult != NULL)
        {
            strcpy(result, *(char **)node->lastResult);
        }
        else
            common_log_1("result for node '%s' is NULL", node->id, LOG_DEBUG);

        break;

    case CS:
        if (assemblyDatas[pos].GetResultFp != NULL)
        {
            ((GetResultMethodFp *)assemblyDatas[pos].GetResultFp)(node->id, result);
        }
        else
            common_log_1("GetResultFp for node '%s' IS NULL!", node->id, LOG_FATAL);
        break;
    }
}
//*************** End exported wrappers to  node workflow functions ****************/

#if defined(__GNUC__) || defined(__gnu_linux__) || defined(__CYGWIN__)
void AddFilesFromDirectoryToTpaList(const char *directory, std::string &tpaList)
{
    const char *const tpaExtensions[] = {
        ".ni.dll", // Probe for .ni.dll first so that it's preferred if ni and il coexist in the same dir
        ".dll",
        ".ni.exe",
        ".exe",
    };

    DIR *dir = opendir(directory);
    if (dir == NULL)
    {
        return;
    }
    std::set<std::string> addedAssemblies;

    // Walk the directory for each extension separately so that we first get files with .ni.dll extension,
    // then files with .dll extension, etc.
    for (int extIndex = 0; extIndex < sizeof(tpaExtensions) / sizeof(tpaExtensions[0]); extIndex++)
    {
        const char *ext = tpaExtensions[extIndex];
        int extLength = strlen(ext);

        struct dirent *entry;

        // For all entries in the directory
        while ((entry = readdir(dir)) != NULL)
        {
            // We are interested in files only
            switch (entry->d_type)
            {
            case DT_REG:
                break;

                // Handle symlinks and file systems that do not support d_type
            case DT_LNK:
            case DT_UNKNOWN:
            {
                std::string fullFilename;

                fullFilename.append(directory);
                fullFilename.append("/");
                fullFilename.append(entry->d_name);

                struct stat sb;
                if (stat(fullFilename.c_str(), &sb) == -1)
                {
                    continue;
                }

                if (!S_ISREG(sb.st_mode))
                {
                    continue;
                }
            }
            break;

            default:
                continue;
            }

            std::string filename(entry->d_name);

            // Check if the extension matches the one we are looking for
            int extPos = filename.length() - extLength;
            if ((extPos <= 0) || (filename.compare(extPos, extLength, ext) != 0))
            {
                continue;
            }

            std::string filenameWithoutExt(filename.substr(0, extPos));

            // Make sure if we have an assembly with multiple extensions present,
            // we insert only one version of it.
            if (addedAssemblies.find(filenameWithoutExt) == addedAssemblies.end())
            {
                addedAssemblies.insert(filenameWithoutExt);

                tpaList.append(directory);
                tpaList.append("/");
                tpaList.append(filename);
                tpaList.append(":");
            }
        }

        // Rewind the directory stream to be able to iterate over it for the next extension
        rewinddir(dir);
    }
    closedir(dir);
}

unsigned int InitializeCoreCLR()
{
    char buff[FILENAME_MAX];
    struct stat st;

    GetCurrentDir(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    netCoreDir = current_working_dir + "/NETCoreRuntime";

    coreClrDir = stat((netCoreDir).c_str(), &st) == 0 ? (netCoreDir).c_str()
                                                      : COMMON_ENGINE_CONFIGURATION.netCorePath;

    string coreclr_path(coreClrDir);
#if defined(__APPLE__)
    coreclr_path.append("/libcoreclr.dylib");
#else
    coreclr_path.append("/libcoreclr.so");
#endif

    void *coreclr = dlopen(coreclr_path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (coreclr == NULL)
    {
        cerr << "failed to open " << coreclr_path << endl;
        cerr << "error: " << dlerror() << endl;
        return -1;
    }

    char app_path[PATH_MAX];
    snprintf(app_path, sizeof(app_path), "%s%s", COMMON_PROJECT_ROOT, "/Implementations");

    coreclr_init = reinterpret_cast<coreclr_initialize_ptr>(dlsym(coreclr, "coreclr_initialize"));
    if (coreclr_init == NULL)
    {
        cerr << "couldn't find coreclr_initialize in " << coreclr_path << endl;
        return -1;
    }

    string tpa_list;
    AddFilesFromDirectoryToTpaList(coreClrDir, tpa_list);

    const char *property_keys[] = {
        "APP_PATHS",
        "TRUSTED_PLATFORM_ASSEMBLIES"};
    const char *property_values[] = {
        // APP_PATHS
        app_path,
        // TRUSTED_PLATFORM_ASSEMBLIES
        tpa_list.c_str()};

    unsigned int domain_id;
    int ret = coreclr_init(
        app_path,                                 // exePath
        "ZenClrDomain",                           // appDomainFriendlyName
        sizeof(property_values) / sizeof(char *), // propertyCount
        property_keys,                            // propertyKeys
        property_values,                          // propertyValues
        &coreclr_handle,                          // hostHandle
        &domain_id                                // domainId
    );

    if (ret < 0)
    {
        cerr << "failed to initialize coreclr. cerr = " << ret << endl;
        return -1;
    }

    coreclr_create_dele = reinterpret_cast<coreclr_create_delegate_ptr>(dlsym(coreclr, "coreclr_create_delegate"));
    if (coreclr_create_dele == NULL)
    {
        cerr << "couldn't find coreclr_create_delegate in " << coreclr_path << endl;
        return -1;
    }
    return domain_id;
}
#endif

/**
* Fills node data's array struct that is passed to managed side
*
* @return	none
*/
void InitNodeDatas()
{
    if (!_areNodeDatasInitialized)
    {
        for (int i = 0; i < COMMON_NODE_LIST_LENGTH; i++)
        {
            strncpy(nodeDatas[i].id, COMMON_NODE_LIST[i]->id, strlen(COMMON_NODE_LIST[i]->id) + 1);
            nodeDatas[i].ptr = COMMON_NODE_LIST[i];
        }

        _areNodeDatasInitialized = 1;
    }
}

EXTERN_DLL_EXPORT int coreclr_init_app_domain()
{
    if (_domainId > 0)
        return 0;

#if defined(_WIN32)
    StartClrRuntime();
    AddTrustedAssemblies();
    SetProbePaths();
    _domainId = CreateAppDomain(L"ZenClrDomain");
#else
    _domainId = InitializeCoreCLR();
#endif
    return 0;
}