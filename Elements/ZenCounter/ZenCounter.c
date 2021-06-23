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
 
#include "ZenCounter.h"
#include "ZenCommon.h"
#include <stdlib.h>
#include <stdio.h>

EXTERN_DLL_EXPORT int onNodePreInit(Node* node)
{
	node->lastResult = malloc(sizeof(int*));
	*node->lastResult = malloc(sizeof(int));
	*((int*)*node->lastResult) = atoi(common_get_node_arg(node, "INITIAL_VALUE"));
	node->lastResultType = RESULT_TYPE_INT;
	return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
	int currentCounterValue = *(int*)*node->lastResult + atoi(common_get_node_arg(node, "COUNTER_STEP"));
	node->isConditionMet = currentCounterValue >= atoi(common_get_node_arg(node, "MAX_VALUE"));

	if (currentCounterValue >= atoi(common_get_node_arg(node, "MAX_VALUE")))
		currentCounterValue = atoi(common_get_node_arg(node, "INITIAL_VALUE"));

	*((int*)*node->lastResult) = currentCounterValue;

	return 0;
}