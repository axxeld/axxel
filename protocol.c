
/**
 *
 * Axxel - Acl memory caching
 *
 * Copyright (c) 2013 Axxel Team
 *
 * Use and distribution licensed under the MIT license.
 * See the LICENSE file for full text.
 *
 * Authors: Andres Gutierrez <andres@phalconphp.com>
 */

#include "json/json.h"

#include "hash.h"

#include "axxel.h"

#include "acl.h"
#include "roles.h"
#include "resources.h"
#include "response.h"

const command commands[] = {

	{ SL("add-role"), p_addrole },
	{ SL("is-role"), p_isrole },
	{ SL("get-roles"), p_getroles },
	{ SL("delete-role"), p_delrole },

	{ SL("add-resource"), p_addresource },
	{ SL("is-resource"), p_isresource },
	{ SL("get-resources"), p_getresources },
	{ SL("delete-resource"), p_delresource },

	{ SL("add-resource-access"), p_addresourceaccess },
	{ SL("is-resource-access"), p_isresourceaccess },
	{ SL("get-resource-accesses"), p_getresourceaccesses },
	{ SL("delete-resource-access"), p_delresourceaccesses },

	{ SL("allow"), p_allow },
	{ SL("deny"), p_deny },
	{ SL("get-accesses"), p_getaccesses },
	{ SL("allowed"), p_isallowed },

	{ SL("default-action"), p_defaultaction },
	{ NULL, 0, NULL }
};

/**
 * Executes the internal function according to the action requested in the parameter
 */
json_object *parse_proto(char *cmd, size_t n) {

	const command *command_list = commands;
	unsigned int action_length, command_found = 0;
	const char *action;
	json_object *new_obj, *action_obj, *response_obj;

	new_obj = json_tokener_parse(cmd);
	if (!new_obj) {
		return p_response_failed_ex("Command cannot be parsed");
	}

	if (json_object_get_type(new_obj) != json_type_object) {
		json_object_put(new_obj);
		return p_response_failed_ex("Commands must be JSON objects");
	}

	action_obj = json_object_object_get(new_obj, "action");
	if (!action_obj) {
		json_object_put(new_obj);
		return p_response_failed_ex("Parameter 'action' is required");
	}

	if (json_object_get_type(action_obj) != json_type_string) {
		json_object_put(new_obj);
		return p_response_failed_ex("Parameter 'action' has an invalid type");
	}

	action = json_object_get_string(action_obj);
	action_length = json_object_get_string_len(action_obj);

	do {
		if (command_list->length == action_length) {
			if (!memcmp(command_list->value, action, action_length)) {
				command_found = 1;
				break;
			}
		}
		++command_list;
	} while (command_list[0].value != NULL);

	if (!command_found) {
		json_object_put(new_obj);
		return p_response_failed_ex("Command was not found");
	}

	response_obj = command_list->func(new_obj);
	json_object_put(new_obj);
	return response_obj;
}
