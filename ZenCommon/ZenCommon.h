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

#pragma once
#include "pthread.h"
#include <stdio.h>
#include <stdarg.h>
#include "log.h"

#if defined(__cplusplus)
#if defined(_WIN32)
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define EXTERN_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#endif
#else
#if defined(_WIN32)
#define EXTERN_DLL_EXPORT __declspec(dllexport)
#define unlink(file) _unlink(file)
#else
#define EXTERN_DLL_EXPORT __attribute__((visibility("default")))
#endif
#endif

#if defined(_WIN32)
#define strdup(s) _strdup(s)
#endif

#if defined(__GNUC__) || defined(__gnu_linux__) || defined(__CYGWIN__)
#define MAX_PATH 4096
#if !defined(NULL)
#define NULL ((void *)0)
#endif
#endif

#define GUID_LENGTH 64
#define NODE_ID_LENGTH 36
#define MAX_DISCONNECTED_NODES_LENGTH NODE_ID_LENGTH * 50

#define LOG_TRACE 0
#define LOG_DEBUG 1
#define LOG_INFO 2
#define LOG_WARN 3
#define LOG_ERROR 4
#define LOG_FATAL 5

typedef enum
{
	RESULT_TYPE_INT,
	RESULT_TYPE_BOOL,
	RESULT_TYPE_DOUBLE,
	RESULT_TYPE_CHAR_ARRAY,
	RESULT_TYPE_JSON_STRING
} result_type;

typedef struct
{
	char *Key;
	char *Value;
} nodeArgs;

struct Implementation
{
	char id[50];
	char type[10];
	void *hDLL;
};

struct buffer
{
	int size;
	int start;
	//int end;  // position of last element
	/* Tracking start and end of buffer would waste
	* one position. A full buffer would always have
	* to leave last position empty or otherwise
	* it would look empty. Instead this buffer uses
	* count to track if buffer is empty or full
	*/
	int count; // number of elements in buffer
			   /* Two ways to make buffer element type opaque
			   * First is by using typedef for the element
			   * pointer. Second is by using void pointer.
			   */
			   /* different types of buffer:
			   int *element;   // array of integers
			   char *element;  // array of characters
			   void *element;  // array of void type (could cast to int, char, etc)
			   char **element; //array of char pointers (array of strings)
			   void **element; // array of void pointers
			   Choosing array of void pointers since it's the most flexible */
	void **element;
};

typedef struct buffer buffer_t;

typedef struct Node
{
	char id[50];
	char implementationId[50];
	int isEventActive;
	void *implementation;
	int trueParentsCnt;
	int falseParentsCnt;
	int trueChildsCnt;
	int falseChildsCnt;
	int disconnectedNodesCnt;
	nodeArgs **args;
	int argsCnt;
	int isStarted;
	char status[10];
	struct Node **ptrTrueChilds;
	struct Node **ptrFalseChilds;
	struct Node **ptrTrueParents;
	struct Node **ptrFalseParents;
	struct Node **disconnectedNodes;
	int isConditionMet;
	char nodeOperator[3];
	time_t started;
	int unregisterEvent;
	int errorCode;
	char errorMessage[512];
	void *implementationContext;
	int isInitialized;
	void **lastResult;
	result_type lastResultType;
	struct Node **nodesToTrigger;
	int nodesToTriggerCnt;
	buffer_t bufferedEvents;
	int isActionable;
	int loopLockId;
	int nodeLockId;
	int pauseNodeConditionId;
	int eventQueueLockId;
	int hasGreenLight;
	int languageUsed;
} Node;

typedef char *(*ptrExecNode)(int (*OnExecNode)(Node *));

struct eventContextParamsStruct
{
	Node *node;
	void *data;
};

typedef struct EngineConfiguration
{
	char *username;
	char *password;
	char *workstationId;
	char *workstationName;
	char *projectId;
	char *nodesVersion;
	char engineVersion[10];
	char projectRoot[256];
	char *netCorePath;
	char *pythonPath;
	char *pythonHome;
	char *pythonModulesPath36;
	char *pythonModulesPath37;
	char *pythonModulesPath38;
	char *AddLibsRootToPath;
	char *updatedBy;
	int displayDebug;
	int logLevel;
	int writeLogToFile;
} EngineConfiguration;
EngineConfiguration engineConfiguration;

typedef enum
{
	// Underlaying node languages
	CPP,
	CS,
	PY
} node_language;

typedef struct
{
	char name[50];
	result_type type;
	void **rows;
} col;

typedef struct
{
	col **cols;
	int colsCount;
	int rowsCount;
} Table;

typedef struct
{
	Table **tables;
	int tablesCount;
} Tables;

#define COMMON_NODE_LIST GetNodeList()
#define COMMON_NODE_LIST_LENGTH GetNodeListLength()
#define COMMON_PROJECT_ROOT GetProjectRoot()
#define COMMON_PROJECT_ID GetProjectId()
#define COMMON_ENGINE_CONFIGURATION GetEngineConfiguration()

char *mystrsep(char **stringp, const char *delim);
void push(buffer_t *buffer, void *data);
void *popqueue(buffer_t *buffer);
void *popstack(buffer_t *buffer);

// LOOP BLOCKER MODE
EXTERN_DLL_EXPORT bool common_is_loop_blocker_mode();
EXTERN_DLL_EXPORT void common_set_is_loop_blocker_mode();

// LOGGING
EXTERN_DLL_EXPORT void common_log_add_fp();
EXTERN_DLL_EXPORT void common_set_log_mode(bool mode);
EXTERN_DLL_EXPORT void common_set_log_level(int log_level);
EXTERN_DLL_EXPORT void common_log_1(const char *fmt, const char *arg1, int log_type);
EXTERN_DLL_EXPORT void common_log_2(const char *fmt, const char *arg1, const char *arg2, int log_type);
EXTERN_DLL_EXPORT void common_log_3(const char *fmt, const char *arg1, const char *arg2, const char *arg3, int log_type);
EXTERN_DLL_EXPORT void common_log_4(const char *fmt, const char *arg1, const char *arg2, const char *arg3, const char *arg4, int log_type);
EXTERN_DLL_EXPORT void common_log_5(const char *fmt, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, int log_type);

EXTERN_DLL_EXPORT void common_wait_debug_signal();
EXTERN_DLL_EXPORT void common_signal_debug_condition();
EXTERN_DLL_EXPORT unsigned common_is_debug_mode_enabled();
EXTERN_DLL_EXPORT void common_set_debug_mode(unsigned isDebugMode);
EXTERN_DLL_EXPORT void common_init_buffer(buffer_t *buffer, int size);
EXTERN_DLL_EXPORT void common_signal_pause_condition(int pauseNodeConditionId);
EXTERN_DLL_EXPORT void common_wait_pause_condition(Node *node, pthread_mutex_t *pause_node_mutex);
EXTERN_DLL_EXPORT int common_init_pause_condition();
EXTERN_DLL_EXPORT void common_init_event_queue_lock(Node *node);
EXTERN_DLL_EXPORT void common_pull_event_from_buffer(Node *node);
EXTERN_DLL_EXPORT void common_push_event_to_buffer(void *context);
EXTERN_DLL_EXPORT void common_init_project(char *project_root, char *project_id, EngineConfiguration engineConfiguration, ptrExecNode execNodeFunct);
EXTERN_DLL_EXPORT char *COMMON_PROJECT_ROOT;
EXTERN_DLL_EXPORT char *COMMON_PROJECT_ID;
EXTERN_DLL_EXPORT Node **COMMON_NODE_LIST;
EXTERN_DLL_EXPORT int COMMON_NODE_LIST_LENGTH;
EXTERN_DLL_EXPORT EngineConfiguration COMMON_ENGINE_CONFIGURATION;
EXTERN_DLL_EXPORT void common_exec_node(Node *node);
EXTERN_DLL_EXPORT int executeAction(Node *node);
EXTERN_DLL_EXPORT char *common_get_node_arg(Node *node, char *key);
EXTERN_DLL_EXPORT void common_set_node_arg(Node *node, char *key, char *value);
EXTERN_DLL_EXPORT void common_set_node_param_arg(Node *node, char *value);
EXTERN_DLL_EXPORT char *common_get_node_param_arg(Node *node);
EXTERN_DLL_EXPORT Node *common_get_node_by_id(char *id);
EXTERN_DLL_EXPORT void common_parse_nodes(Node **childsArr, char **childs, int cnt);
EXTERN_DLL_EXPORT int common_is_string_in_array(char **arr, char *str, int arrLength);
EXTERN_DLL_EXPORT void common_str_split(const char *str, const char *delim, int *numtokens, char ***tokens);
EXTERN_DLL_EXPORT char *common_str_replace(char *orig, char *rep, char *with);
EXTERN_DLL_EXPORT char *common_str_decode(char *orig);
EXTERN_DLL_EXPORT void common_concatenate_string(char *original, char *add, int cnt);
EXTERN_DLL_EXPORT void common_free_splitted_string(char **splittedString, int cnt);
EXTERN_DLL_EXPORT int common_does_condition_exists(int conditions[], int condition, int conditionCnt);
EXTERN_DLL_EXPORT int common_node_exists(Node **nodeArr, Node *node, int listCnt);
EXTERN_DLL_EXPORT void common_initialize_node_list(int nodeListCnt);
EXTERN_DLL_EXPORT void common_add_node_to_list(Node *node);
EXTERN_DLL_EXPORT void common_generate_guid(char guid[GUID_LENGTH], int number_of_blocks);
EXTERN_DLL_EXPORT int common_string_ends_with(const char *str, const char *suffix);
EXTERN_DLL_EXPORT int common_remove_directory(const char *path);
EXTERN_DLL_EXPORT void common_mkdir_recursively(const char *dir);
EXTERN_DLL_EXPORT void common_get_files(const char *path, char **files, int *filesCnt);
EXTERN_DLL_EXPORT size_t common_getline(char **lineptr, size_t *n, FILE *stream);
EXTERN_DLL_EXPORT int common_directory_exists(const char *path);
EXTERN_DLL_EXPORT void common_json_dump_table(Tables *tables);
EXTERN_DLL_EXPORT void TestDump();
EXTERN_DLL_EXPORT void common_register_log_f(void (*ptr_log_f)(char *, int));
EXTERN_DLL_EXPORT void common_call_log_f(char *message, int type);
EXTERN_DLL_EXPORT Node **common_fill_disconnected_nodes(char *node_ids, int *nodesToExecuteCnt);
EXTERN_DLL_EXPORT char *common_replace_word(const char *s, const char *oldW, const char *newW);

EXTERN_DLL_EXPORT void common_set_is_lib_mode(int isLibMode);
EXTERN_DLL_EXPORT int common_get_lib_mode();