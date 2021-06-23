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

#include "ZenShellExecuter.h"
#include "ZenCommon.h"
#include <stdlib.h>
#include <stdio.h>

#if defined (_WIN32)
#define  popen _popen
#define  pclose _pclose
#endif

EXTERN_DLL_EXPORT int onImplementationLoad()
{
	return 0;
}

EXTERN_DLL_EXPORT int executeAction(Node *node)
{
	common_mkdir_recursively(common_get_node_arg(node, "RESULTS_PATH"));
	
	FILE * proc;
	char ch;

	proc = popen(common_get_node_arg(node, "SHELL_COMMAND"), "r");

	if( proc == NULL)
	{
		puts("Unable to open process");
		return(1);
	}
    	
	while((ch=fgetc(proc)) != EOF)
	{
		putchar(ch);
		if (ch == '\n')
			fflush(stdout);
	}
	pclose(proc);
	
	node->isConditionMet = 1;
	return 0;
}
