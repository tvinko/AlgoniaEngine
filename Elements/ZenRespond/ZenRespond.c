//https://github.com/DaveGamble/cJSON/issues/154

///usr/local/Cellar
// libczmq.4.dylib to /usr/local/lib
#include "ZenRespond.h"

EXTERN_DLL_EXPORT int onNodeComplete(Node *completedNode, Node *currentNode)
{
	// Check if currentNode is last node (without childs)
	if (strcmp(_current_node->id, currentNode->id) == 0 && completedNode->ptrTrueChilds == NULL)
	{
		_completed_node = completedNode;
		pthread_mutex_lock(&lock);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&lock);
	}

	return 0;
}

EXTERN_DLL_EXPORT int onNodePreInit(Node *node)
{
	_completed_node = NULL;
	_current_node = node;
	node->lastResult = NULL;
	node->lastResultType = RESULT_TYPE_CHAR_ARRAY;
	return 0;
}

EXTERN_DLL_EXPORT Node **getNodesToExecute(Node *node, int *nodesToExecuteCnt)
{
	int numNodesToExecute = 0;
	char **nodesToExecute = NULL;

	return common_fill_disconnected_nodes(common_get_node_arg(node, "APIS"), nodesToExecuteCnt);
}

//########### START JS CALLBACKS##########################
void exec(char *node_id)
{
	common_log_1("RESPONDER: START exec node '%s'", node_id, LOG_DEBUG);
	if (_result != NULL)
		free(_result);

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

	common_log_1("RESPONDER: END exec node '%s'", node_id, LOG_DEBUG);
}

void get_result_raw(char *node_id)
{
	common_log_1("RESPONDER: START get result for node '%s'", node_id, LOG_DEBUG);
	Node *node = common_get_node_by_id(node_id);
	if (node != NULL)
	{
		int len = coreclr_get_result_len(node);
		if (len > -1)
		{
			if (_result != NULL)
				free(_result);

			_result = malloc(len * sizeof(char) + 1);
			coreclr_get_result(node, _result);
		}
	}
	else
		common_log_1("RESPONDER: result for node '%s' IS NULL", node_id, LOG_WARN);

	common_log_1("RESPONDER: END get result for node '%s'", node_id, LOG_DEBUG);
}

void set_node_arg(char *node_id, char *arg_val)
{
	common_log_1("RESPONDER: START set arg for node '%s'", node_id, LOG_DEBUG);

	if (_result != NULL)
		free(_result);

	_result = malloc(1 * sizeof(char) + 1);
	Node *node = common_get_node_by_id(node_id);
	if (node != NULL)
	{
		common_set_node_param_arg((Node *)node, arg_val);
		strcpy(_result, "0");
	}
	else
	{
		strcpy(_result, "1");
		common_log_1("RESPONDER: node '%s' IS NULL", node_id, LOG_WARN);
	}

	common_log_1("RESPONDER: END set arg for node '%s'", node_id, LOG_DEBUG);
}

void get_node_arg(char *node_id)
{
	common_log_1("RESPONDER: START get arg for node '%s'", node_id, LOG_DEBUG);
	Node *node = common_get_node_by_id(node_id);
	if (node != NULL)
	{
		char *tmp = common_get_node_param_arg((Node *)node);

		if (_result != NULL)
			free(_result);

		_result = malloc(strlen(tmp) * sizeof(char) + 1);
		strcpy(_result, tmp);
	}
	else
		common_log_1("RESPONDER: node '%s' IS NULL", node_id, LOG_WARN);

	common_log_1("RESPONDER: START get arg for node '%s'", node_id, LOG_DEBUG);
}
//########### END JS CALLBACKS#############################

void set_node_arg_wrapper(cJSON *root, char *cNodeId)
{
	cJSON *argVal = cJSON_GetObjectItem(root, "argVal");

	if (!cJSON_IsString(argVal))
		common_log_1("ArgVal is not valid JSON object%s", "...", LOG_ERROR);

	char *cArgVal = malloc(strlen(argVal->valuestring) + sizeof(""));
	strcpy(cArgVal, argVal->valuestring);

	set_node_arg(cNodeId, cArgVal);
	free(cArgVal);
}

EXTERN_DLL_EXPORT int onEngineStarted(Node *node)
{
	zsock_t *responder = zsock_new_rep("tcp://*:5000");
	if (responder == NULL)
	{
		common_log_1("Cannot bind ZMQ receiver%s", "...", LOG_ERROR);
		return 1;
	}

	common_log_1("ZMQ receiver listening on port 5000%s", "...", LOG_DEBUG);
	while (1)
		main_loop(responder);

	zsock_destroy(&responder);
	return 0;
}

void main_loop(zsock_t *responder)
{
	common_log_1("ZMQ receiver waiting for request%s", "...", LOG_DEBUG);
	char *jsonMsg = zstr_recv(responder);

	cJSON *root = cJSON_Parse(jsonMsg);

	zstr_free(&jsonMsg);

	cJSON *method = cJSON_GetObjectItem(root, "method");
	cJSON *nodeId = cJSON_GetObjectItem(root, "nodeId");

	if (!cJSON_IsString(method))
		common_log_1("Method is not valid JSON object%s", "...", LOG_ERROR);

	if (!cJSON_IsString(nodeId))
		common_log_1("Node is not valid JSON object%s", "...", LOG_ERROR);

	char *cMethod = malloc(strlen(method->valuestring) + sizeof(""));
	strcpy(cMethod, method->valuestring);

	char *cNodeId = malloc(strlen(nodeId->valuestring) + sizeof(""));
	strcpy(cNodeId, nodeId->valuestring);

	common_log_2("Request arrived. Method: '%s', NodeId: '%s'", cMethod, cNodeId, LOG_DEBUG);

	if (
		strcmp(cMethod, "exec") == 0 || strcmp(cMethod, "exec_get_result") == 0 || strcmp(cMethod, "set_arg_exec_get_result") == 0)
	{
		if (strcmp(cMethod, "set_arg_exec_get_result") == 0)
			set_node_arg_wrapper(root, cNodeId);

		exec(cNodeId);

		if (strcmp(cMethod, "exec_get_result") == 0 || strcmp(cMethod, "set_arg_exec_get_result") == 0)
			get_result_raw(_completed_node->id);
	}
	else if (strcmp(cMethod, "get_result") == 0)
	{
		get_result_raw(cNodeId);
	}
	else if (strcmp(cMethod, "set_arg") == 0 || strcmp(cMethod, "set_arg_exec") == 0)
	{
		set_node_arg_wrapper(root, cNodeId);

		if (strcmp(cMethod, "set_arg_exec") == 0)
			exec(cNodeId);
	}
	else if (strcmp(cMethod, "get_arg") == 0)
		get_node_arg(cNodeId);

	if (_result == NULL)
	{
		_result = malloc(4 * sizeof(char) + 1);
		strcpy(_result, "NULL");
	}

	free(cMethod);
	free(cNodeId);
	cJSON_Delete(root);

	zstr_send(responder, _result);
	free(_result);
	_result = NULL;
}