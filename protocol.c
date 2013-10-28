
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
	JSString* u16_txt;
	unsigned int length;
	char *txt;

	fprintf(stderr, "%s\n", cmd);

	JS_BeginRequest(context->jsContext);

	ok = JS_EvaluateScript(context->jsContext, JS_GetGlobalObject(context->jsContext), cmd, strlen(cmd), "TEST", 1, &rval);

	JS_EndRequest(context->jsContext);

	if (ok) {

		if (JSVAL_IS_INT(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "status", json_object_new_string("ok"));
			json_object_object_add(response, "result", json_object_new_int(JSVAL_TO_INT(rval)));
			return response;
		}

		if (JSVAL_IS_DOUBLE(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "status", json_object_new_string("ok"));
			json_object_object_add(response, "result", json_object_new_double(JSVAL_TO_DOUBLE(rval)));
			return response;
		}

		if (JSVAL_IS_BOOLEAN(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "status", json_object_new_string("ok"));
			json_object_object_add(response, "result", json_object_new_boolean(JSVAL_TO_BOOLEAN(rval)));
			return response;
		}

		if (JSVAL_IS_STRING(rval)) {

			u16_txt = JSVAL_TO_STRING(rval);
			length = JS_GetStringEncodingLength(context->jsContext, u16_txt);
			txt = malloc(sizeof(char)*(length + 1));
			JS_EncodeStringToBuffer(u16_txt, txt, length);

			response = json_object_new_object();
			json_object_object_add(response, "status", json_object_new_string("ok"));
			json_object_object_add(response, "result", json_object_new_string_len(txt, length));

			free(txt);
			return response;
		}

		if (JSVAL_IS_NULL(rval)) {
			response = json_object_new_object();
			json_object_object_add(response, "status", json_object_new_string("ok"));
			return response;
		}

		fprintf(stderr, "%d", JS_TypeOfValue(context->jsContext, rval));

		return p_response_failed_ex("Unknown type");
	}

	return p_response_failed_ex("Commands must be JSON objects");
}
