#include <stdlib.h>
#include "ZenGUI.h"
#include "cJSON.h"
#if defined(__APPLE__)
#include "string.h"
#endif
char *_result = NULL;

// Declaration of thread condition variable
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

Node *_current_node;

//############ START ALGONIA SYSTEM AREA###################
EXTERN_DLL_EXPORT Node **getNodesToExecute(Node *node, int *nodesToExecuteCnt)
{
  char *val = common_get_node_arg((Node *)node, "NODES_TO_EXECUTE");
  return common_fill_disconnected_nodes(val, nodesToExecuteCnt);
}

EXTERN_DLL_EXPORT int onNodeComplete(Node *completedNode, Node *currentNode)
{
  // Check if currentNode is last node (without childs)
  if (strcmp(_current_node->id, currentNode->id) == 0 && completedNode->ptrTrueChilds == NULL)
  {
    pthread_cond_signal(&cond);
  }

  return 0;
}
//############ END   ALGONIA SYSTEM AREA###################

//########### START JS CALLBACKS##########################
void exec(char *node_id)
{
  _result = malloc(1 * sizeof(char) + 1);

  Node *node = common_get_node_by_id(node_id);
  if (node != NULL)
  {
    common_exec_node(node);
    if (node->ptrTrueChilds != NULL)
    {
      pthread_mutex_lock(&lock);
      pthread_cond_wait(&cond, &lock);
      pthread_mutex_unlock(&lock);
    }
    strcpy(_result, "0");
  }
  else
    strcpy(_result, "1");
}

void get_result_raw(char *node_id)
{
  Node *node = common_get_node_by_id(node_id);
  if (node != NULL)
  {
    int len = coreclr_get_result_len(node);
    _result = malloc(len * sizeof(char) + 1);
    coreclr_get_result(node, _result);
  }
}

void set_node_arg(char *node_id, char *arg_val)
{
  _result = malloc(1 * sizeof(char) + 1);
  Node *node = common_get_node_by_id(node_id);
  if (node != NULL)
  {
    common_set_node_param_arg((Node *)node, arg_val);
    strcpy(_result, "0");
  }
  else
    strcpy(_result, "1");
}

void get_node_arg(char *node_id)
{
  Node *node = common_get_node_by_id(node_id);
  if (node != NULL)
  {
    char *tmp = common_get_node_param_arg((Node *)node);
    _result = malloc(strlen(tmp) * sizeof(char) + 1);
    strcpy(_result, tmp);
  }
}
//########### END JS CALLBACKS#############################

char *algonia_callback(char *arg)
{
  cJSON *root = cJSON_Parse(arg);

  cJSON *cc_action = cJSON_GetArrayItem(root, 0);
  if (
      strcmp(cc_action->valuestring, "exec") == 0 || strcmp(cc_action->valuestring, "exec_get_result") == 0 || strcmp(cc_action->valuestring, "set_arg_exec_get_result") == 0)
  {
    cJSON *nodeId = cJSON_GetArrayItem(root, 1);
    cJSON *arg_val = cJSON_GetArrayItem(root, 2);

    if (strcmp(cc_action->valuestring, "set_arg_exec_get_result") == 0)
      set_node_arg(nodeId->valuestring, arg_val->valuestring);

    exec(nodeId->valuestring);

    if (strcmp(cc_action->valuestring, "exec_get_result") == 0)
      get_result_raw(nodeId->valuestring);
  }
  else if (strcmp(cc_action->valuestring, "get_result") == 0)
  {
    cJSON *nodeId = cJSON_GetArrayItem(root, 1);
    get_result_raw(nodeId->valuestring);
  }
  else if (strcmp(cc_action->valuestring, "set_arg") == 0 || strcmp(cc_action->valuestring, "set_arg_exec") == 0)
  {
    cJSON *nodeId = cJSON_GetArrayItem(root, 1);
    cJSON *arg_val = cJSON_GetArrayItem(root, 2);

    set_node_arg(nodeId->valuestring, arg_val->valuestring);
    if (strcmp(cc_action->valuestring, "set_arg_exec") == 0)
      exec(nodeId->valuestring);
  }
  else if (strcmp(cc_action->valuestring, "get_arg") == 0)
  {
    cJSON *nodeId = cJSON_GetArrayItem(root, 1);
    get_node_arg(nodeId->valuestring);
  }

  if (_result == NULL)
  {
    _result = malloc(4 * sizeof(char) + 1);
    strcpy(_result, "NULL");
    return _result;
  }
  return _result;
}

void algonia_free()
{
  free(_result);
}

EXTERN_DLL_EXPORT int onEngineStarted(Node *node)
{
  char web_root_path[256];

  _current_node = node;

  snprintf(web_root_path, sizeof(web_root_path), "%s%s%s", COMMON_PROJECT_ROOT, "/", common_get_node_arg(node, "WEB_ROOT"));
  init_web_view(&algonia_callback, &algonia_free, web_root_path);
  return 0;
}