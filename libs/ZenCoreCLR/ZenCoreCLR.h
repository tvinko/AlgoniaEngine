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

#pragma once

// Describes where managed Elements results are handled
typedef enum
{
    // result is handled on native side. Those are Eventable Elements
    // that are storing managed events into unmanaged buffer, or
    // Actionable Elements that are assigning results on native side
	NOT_MANAGED,
    // all other Actionable managed Elements
	IS_MANAGED
} is_element_managed;

// Describes if Element can contain virtually connected Elements
// Those are typically scripts and condition evaluators,
// where Elements are executed from script instead of workflow connections
typedef enum
{
	CONTAIN_DYN_ELEMENTS,
	DOES_NOT_CONTAIN_DYN_ELEMENTS
} contains_dynamic_elements;

void InitNodeDatas();
EXTERN_DLL_EXPORT int coreclr_init_app_domain();
EXTERN_DLL_EXPORT int coreclr_create_delegates(char* fileName,char* className, contains_dynamic_elements can_contain_dyn_elements);
EXTERN_DLL_EXPORT void coreclr_init_managed_nodes(int pos, Node* node, is_element_managed is_managed);
EXTERN_DLL_EXPORT void coreclr_execute_action(int pos, Node* node, char* result);
EXTERN_DLL_EXPORT void coreclr_on_node_init(int pos, Node* node, char* result);
EXTERN_DLL_EXPORT void coreclr_get_dynamic_nodes(int pos, Node* node, char **result, is_element_managed is_managed);
EXTERN_DLL_EXPORT void coreclr_get_result(Node* node, char* result);
EXTERN_DLL_EXPORT int coreclr_get_result_len(Node* node);