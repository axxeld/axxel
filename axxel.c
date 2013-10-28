
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
#include <unistd.h>

#include "hash.h"

#include "axxel.h"
#include "protocol.h"

JSBool js_print(JSContext *cx, uintN argc, jsval *vp);

static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSFunctionSpec myjs_global_functions[] =
{
	JS_FS("print",   js_print,   1, 0),
	JS_FS_END
};

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	fprintf(stderr, "%s:%u:%s\n", report->filename ? report->filename : "<no filename>", (unsigned int) report->lineno, message);
}

JSBool js_print(JSContext *cx, uintN argc, jsval *vp)
{
	JSString* u16_txt;
	unsigned int length;
	char *txt;

	if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &u16_txt)) {
		return JS_FALSE;
	}

	length = JS_GetStringEncodingLength(cx, u16_txt);
	txt = alloca(sizeof(char)*(length + 1));
	JS_EncodeStringToBuffer(u16_txt, txt, length);

	printf("%.*s\n", length, txt);

	//free(txt);
	return JS_TRUE;
}

int loadScript(axxel_context *context, const char *file_name)
{

	jsval rval;
	JSObject *script;

	script = JS_CompileFile(context->jsContext, JS_GetGlobalObject(context->jsContext), file_name);
	JS_ExecuteScript(context->jsContext, JS_GetGlobalObject(context->jsContext), script, &rval);
}

int main(int argc, char **argv)
{

	/* JS variables. */
	char *host = NULL, *port = NULL;
	axxel_context *context;
	int opt;

	while ((opt = getopt(argc, argv, "p:h:")) != -1) {
		switch (opt) {
			case 'h':
				host = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s -h host -p port\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

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
		fprintf(stderr, "Cannot register new compartment\n");
		return -1;
	}

	if (!JS_DefineFunctions(context->jsContext, context->global, myjs_global_functions)) {
		fprintf(stderr, "Cannot register custom functions\n");
		return -1;
	}

	/* Populate the global object with the standard globals, like Math, Object and Array. */
	if (!JS_InitStandardClasses(context->jsContext, context->global)) {
		fprintf(stderr, "Cannot register standard objects\n");
		return -1;
	}

	loadScript(context, "acl.js");
	loadScript(context, "roles.js");
	loadScript(context, "resources.js");
	loadScript(context, "axxel.js");

	start_server(context, host, port);
	return 0;
}

