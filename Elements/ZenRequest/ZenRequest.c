#include "ZenRequest.h"

EXTERN_DLL_EXPORT int onNodePreInit(Node *node)
{
	node->lastResult = NULL;
	node->lastResultType = RESULT_TYPE_CHAR_ARRAY;
	return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
	zsock_t *requester = zsock_new_req(common_get_node_arg(node, "URL"));
	zsock_set_linger(requester, 10);

	Node *arg_node = common_get_node_by_id(common_get_node_arg(node, "ARG_NODE"));
	char *result = (char *)malloc(coreclr_get_result_len(arg_node) * sizeof(char) + 1);
	coreclr_get_result(arg_node, result);

	char *c_api_call;
	construct_api_call(node, &c_api_call, result);
	free(result);

	zstr_send(requester, c_api_call);

	free(c_api_call);

	char *string = zstr_recv(requester);
	
	if (node->lastResult != NULL)
	{
		free(*node->lastResult);
		free(node->lastResult);
	}
	node->lastResult = (char **)(malloc(sizeof(char *)));
	*node->lastResult = (char *)malloc(sizeof(char) * (strlen(string) + 1));
	strcpy(*node->lastResult, string);

	zstr_free(&string);

	zsock_destroy(&requester);
	return 0;
}

void construct_api_call(Node *node, char **c_api_call, char *result)
{
	*c_api_call = malloc(strlen(common_get_node_arg(node, "API_KEY")) + strlen(result) + 2);
	strcpy(*c_api_call, "");

	common_concatenate_string(*c_api_call, common_get_node_arg(node, "API_KEY"), strlen(common_get_node_arg(node, "API_KEY")));
	common_concatenate_string(*c_api_call, "^", 1);
	common_concatenate_string(*c_api_call, result, strlen(result));
}