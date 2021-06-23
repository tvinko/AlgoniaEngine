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

#include "ZenGwSystemInfoWrapper.h"
#include "ZenCommon.h"
#include "ZenCoreCLR.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EXTERN_DLL_EXPORT int onNodePreInit(Node* node)
{
	coreclr_init_app_domain();
    node->languageUsed = CS;
	node->implementationContext = malloc(sizeof(int));
	*((int*)(node->implementationContext)) = coreclr_create_delegates("ZenGwSystemInfo", DOES_NOT_CONTAIN_DYN_ELEMENTS);
	coreclr_init_managed_nodes(*((int*)(node->implementationContext)), node, IS_MANAGED);
	return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
	char* result = malloc(100 * sizeof(char));
	coreclr_execute_action(*((int*)(node->implementationContext)), node, result);
	node->isConditionMet = 1;
	free(result);
	return 0;
}