#include <czmq.h>
#include "ZenCommon.h"
#include "ZenCoreCLR.h"
#include "cJSON.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

char *_result = NULL;
Node *_current_node;
Node *_completed_node;

void main_loop(zsock_t *responder);
void exec(char *node_id);
void get_result_raw(char *node_id);
void set_node_arg(char *node_id, char *arg_val);
void get_node_arg(char *node_id);
void set_node_arg_wrapper(cJSON *root, char *cNodeId);