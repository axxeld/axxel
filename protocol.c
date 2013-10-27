
/**
 *
 * Axxel - Acl memory caching
 *
 * Copyright (c) 2013 Axxel Team
 *
 * Use and distribution licensed under the MIT license.
 * See the LICENSE file for full text.
 *
 * Authors: Andres Gutierrez <andres@axxeld.com>
 */

#include <stdio.h>

#include "hash.h"

#include "axxel.h"

#include "response.h"

/**
 * Executes the internal function according to the action requested in the parameter
 */
json_object *execute_proto(axxel_context *context, char *cmd, size_t n) {

	json_object *response;
	JSBool ok;
    jsval rval;
    JSObject *script;

    fprintf(stderr, "%s\n", cmd);

	ok = JS_EvaluateScript(context->jsContext, JS_GetGlobalObject(context->jsContext), cmd, strlen(cmd), "TEST", 1, &rval);

	if (ok) {
		if (JSVAL_IS_INT(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "count", json_object_new_int(JSVAL_TO_INT(rval)));
			return response;
		}
		if (JSVAL_IS_DOUBLE(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "count", json_object_new_double(JSVAL_TO_DOUBLE(rval)));
			return response;
		}
	}

	return p_response_failed_ex("Commands must be JSON objects");

	/*new_obj = json_tokener_parse(cmd);
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

	response_obj = command_list->func(acl_lists, new_obj);
	json_object_put(new_obj);
	return response_obj;*/
}

int shutdown_proto()
{
	/* Your application code here. This may include JSAPI calls
       to create your own custom JS objects and run scripts. */
    //test(cx);

    /* Cleanup. */
    //JS_DestroyContext(cx);
    //JS_DestroyRuntime(rt);
    //JS_ShutDown();
    return 0;
}
