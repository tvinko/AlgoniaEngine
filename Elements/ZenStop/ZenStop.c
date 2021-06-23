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
#include "ZenStop.h"

EXTERN_DLL_EXPORT int onImplementationLoad(char implementationId[50])
{
	return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
	set_result(node);
	node->isConditionMet = 1;
	return 0;
}

void set_result(Node *current_node)
{
	Node *node = common_get_node_by_id(common_get_node_arg(current_node, "PARENT_NODE_ID"));
    
	int len = coreclr_get_result_len(node);
	char *result = (char *)malloc(len * sizeof(char) + 1);
    coreclr_get_result(node, result);
	current_node->lastResult = (char **)malloc(sizeof(char *) * len);
	*current_node->lastResult = (char *)malloc(sizeof(char) * len);
	strncpy(*current_node->lastResult, result, len);
    free(result);
}