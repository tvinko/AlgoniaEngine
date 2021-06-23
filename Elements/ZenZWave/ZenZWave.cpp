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

#include "ZenZWave.h"
#include "ZenCommon.h"
#include "ZenZWaveCommon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

EXTERN_DLL_EXPORT int onNodePreInit(Node *node)
{
	bool initFailed = InitializeDriver(".", "\\\\.\\COM7");
	return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
	float f = NULL;
	HandleGets(23, 0x31, &f, RESULT_TYPE_FLOAT);
	float result = *((float *)&f);

	node->isConditionMet = 1;
	return 0;
}