
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mozilla/js/js/src/jsapi.h"
#include "json/json.h"

#include "hash.h"

#include "axxel.h"
#include "protocol.h"

static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	fprintf(stderr, "%s:%u:%s\n", report->filename ? report->filename : "<no filename>", (unsigned int) report->lineno, message);
}

int loadScript(axxel_context *context, char *file_name)
{
	FILE *fp;
	char ch;
	char *program;
	int i, length;
	JSBool ok;
    jsval rval;
    JSObject *script;

	fp = fopen(file_name, "r");
	if (!fp) {
		fprintf(stderr, "Cant open file\n");
		return 0;
	}

	length = 1024;
	program = malloc(sizeof(char) * length);

	i = 0;
	while (!feof(fp)) {
		ch = fgetc(fp);
		if (i == length) {
			length += 1024;
			program = realloc(program, sizeof(char) * length);
		}
		program[i++] = ch;
	}
	program[i - 1] = '\0';
	fclose(fp);

	ok = JS_EvaluateScript(context->jsContext, JS_GetGlobalObject(context->jsContext), program, i -1, file_name, 1, &rval);

	free(program);
}

int main(int argc, char **argv) {

	/* JS variables. */
	axxel_context *context;

	/* Create an Axxel Context */
	context = malloc(sizeof(axxel_context));

	/* Create a JS runtime. */
	context->runtime = JS_NewRuntime(8L * 1024L * 1024L);
	if (context->runtime == NULL) {
		fprintf(stderr, "Cannot create Javascript runtime\n");
		return -1;
	}

	/* Create a context. */
	context->jsContext = JS_NewContext(context->runtime, 8192);
	if (context->jsContext == NULL) {
		fprintf(stderr, "Cannot create Javascript context\n");
		return -1;
	}

	JS_SetOptions(context->jsContext, JSOPTION_VAROBJFIX | JSOPTION_JIT | JSOPTION_METHODJIT);
	JS_SetVersion(context->jsContext, JSVERSION_LATEST);
	JS_SetErrorReporter(context->jsContext, reportError);

	/* Create the global object in a new compartment. */
	context->global = JS_NewCompartmentAndGlobalObject(context->jsContext, &global_class, NULL);
	if (context->global == NULL) {
		return -1;
	}

	//if (!JS_DefineFunctions(cx, global, myjs_global_functions))
	  //  return JS_FALSE;

	/* Populate the global object with the standard globals, like Math, Object and Array. */
	if (!JS_InitStandardClasses(context->jsContext, context->global)) {
		return -1;
	}

	loadScript(context, "acl.js");
	loadScript(context, "roles.js");
	loadScript(context, "resources.js");
	loadScript(context, "axxel.js");

	start_server(context);
	return 0;
}

