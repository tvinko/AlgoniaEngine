/*************************************************************************
 * Copyright (c) 2015, 2019 Zenodys BV
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

#include "ZenLicenceCheckerWrapper.h"
#include "ZenCommon.h"
#include "ZenCoreCLR.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Node* _nodeIds[100];
int _nodeListCnt = 0;

EXTERN_DLL_EXPORT int onSubscribeNodeToEvent(Node* node)
{
	_nodeIds[_nodeListCnt] = malloc(sizeof(Node));
	_nodeIds[_nodeListCnt] = node;
	_nodeListCnt++;
	return 0;
}

EXTERN_DLL_EXPORT int onNodeEvent(Node* node, char* data)
{
	struct eventContextParamsStruct event_context;
	((Node*)node)->isConditionMet = 1;
	((Node*)node)->lastResultType = RESULT_TYPE_CHAR_ARRAY;
	
	event_context.data = (char*) malloc(sizeof(char));
	strncpy((char*)event_context.data, data, strlen(data) + 1);
	event_context.node = node;
		
	common_push_event_to_buffer(&event_context);
	return 0;
}

EXTERN_DLL_EXPORT int onNodePreInit(Node* node)
{
	coreclr_init_app_domain();
    node->languageUsed = CS;
	node->implementationContext = malloc(sizeof(int));
	*((int*)(node->implementationContext)) = coreclr_create_delegates("ZenLicenceChecker", DOES_NOT_CONTAIN_DYN_ELEMENTS);
	coreclr_init_managed_nodes(*((int*)(node->implementationContext)), node, NOT_MANAGED);
	return 0;
}

