
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
#include "js/js/src/jsapi.h"

#include "json-c/json.h"

#include "hash.h"

#include "axxel.h"

#include "acl.h"
#include "roles.h"
#include "resources.h"
#include "response.h"

static JSClass global_class =
{
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

const proto_command commands[] = {

	{ SL("create-acl"), p_createacl },

	/*{ SL("add-role"), p_addrole },
	{ SL("is-role"), p_isrole },
	{ SL("get-roles"), p_getroles },
	{ SL("delete-role"), p_delrole },*/

	{ SL("add-resource"), p_addresource },
	{ SL("is-resource"), p_isresource },
	{ SL("get-resources"), p_getresources },
	{ SL("count-resources"), p_countresources },
	{ SL("delete-resource"), p_delresource },

	{ SL("add-resource-access"), p_addresourceaccess },
	{ SL("is-resource-access"), p_isresourceaccess },
	{ SL("get-resource-accesses"), p_getresourceaccesses },
	{ SL("delete-resource-access"), p_delresourceaccesses },

	/*{ SL("allow"), p_allow },
	{ SL("deny"), p_deny },
	{ SL("get-accesses"), p_getaccesses },
	{ SL("allowed"), p_isallowed },

	{ SL("default-action"), p_defaultaction },*/
	{ NULL, 0, NULL }
};

/**
 * Executes the internal function according to the action requested in the parameter
 */
json_object *parse_proto(JSContext *cx, p_hash_table *acl_lists, char *cmd, size_t n) {

	const proto_command *command_list = commands;
	//unsigned int action_length, command_found = 0;
	//const char *action;
	//json_object *new_obj, *action_obj, *response_obj;
	json_object *response;
	JSBool ok;
    jsval rval;
    JSObject *script;

    fprintf(stderr, "%s\n", cmd);

	ok = JS_EvaluateScript(cx, JS_GetGlobalObject(cx), cmd, strlen(cmd), "TEST", 1, &rval);

	if (ok) {
		if (JSVAL_IS_INT(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "count", json_object_new_int(JSVAL_TO_INT(rval)));
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

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
    fprintf(stderr, "%s:%u:%s\n", report->filename ? report->filename : "<no filename>", (unsigned int) report->lineno, message);
}

JSContext *init_proto()
{
	/* JS variables. */
    JSRuntime *rt;
    JSContext *cx;
    JSObject  *global;

    /* Create a JS runtime. */
    rt = JS_NewRuntime(8L * 1024L * 1024L);
    if (rt == NULL)
        return NULL;

    /* Create a context. */
    cx = JS_NewContext(rt, 8192);
    if (cx == NULL)
        return NULL;

    JS_SetOptions(cx, JSOPTION_VAROBJFIX | JSOPTION_JIT | JSOPTION_METHODJIT);
    JS_SetVersion(cx, JSVERSION_LATEST);
    JS_SetErrorReporter(cx, reportError);

    /* Create the global object in a new compartment. */
    global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
    if (global == NULL)
        return NULL;

    //if (!JS_DefineFunctions(cx, global, myjs_global_functions))
      //  return JS_FALSE;

    /* Populate the global object with the standard globals,
       like Object and Array. */
    if (!JS_InitStandardClasses(cx, global))
        return NULL;

    return cx;
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
