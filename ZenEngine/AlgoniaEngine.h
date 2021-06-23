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
#include "dirent.h"
#include "ZenCommon.h"
#if defined(__APPLE__)
#include <unistd.h>
#endif

// Max number of child nodes
#define MAX_CHILD_NODES_COUNT 100

// Max number of all nodes
#define MAX_NODES_COUNT 1000

//Length of project Id
#define PROJECT_ID_LENGTH 37

//Max string length of false and true childs
#define FIRST_LEVEL_RELATIONS_STRING_LENGTH 512

//Function declarations
typedef int (*ptrOnImplementationLoad)(char[50]);
typedef Node **(*ptrOnGetNodesToExecute)(Node *, int *);
typedef int (*ptrOnNodeInit)(Node *);
typedef int (*ptrOnEngineStarted)(Node *);
typedef int (*ptrOnNodePreInit)(Node *);
typedef int (*ptrExecuteAction)(Node *);
typedef int (*ptrSubscribeNodeToEvent)(Node *);
typedef int (*ptrOnNodeComplete)(Node *, Node *);

void SetLogging();
void FireOnEngineStartedEvent();
int execNode(Node *node);
void StartNodeCore(Node *node, int *isNodeFirstFires);
void RunNodeInterfaces(Node *node);
void StartNode(void *context);
void StartLoops();
void StartOrSignalNodes(Node **nodes, int startNodesCnt, Node **stopNodeList, int *iStopNodesListCnt);
void AddStopParentsToList(Node **nodes, int stopNodesCnt, Node **stopNodeList, int *iStopNodesListCnt);
void OnNodeFinish(Node *node);
void ReadZenFile(char zenFileName[MAX_PATH], char **input);
void SetProjectId(const char *sDir);
int FillImplementationList();
void FillNodeList();
void FillRelationList();
void SyncChilds(Node *currentNode, int loopLockId);
void SyncLoops();
void SetPaths(char elements_file_path[256]);
void ExecuteMainThreadActions();
void SyncLoop();
void MakeVirtualconnections();
void SafeNodeStart(Node *node);
void ReadEngineConfiguration();
void DeleteObsoleteNodeFiles();
int start_engine(char elements_file_path[256], int is_lib_mode);

EXTERN_DLL_EXPORT char *execute_template(char elements_file_path[256]);