
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

#ifndef HAVE_AXXEL_H
#define HAVE_AXXEL_H 1

#include "jsapi.h"
#include "json-c/json.h"

#define SL(ch) ch, sizeof(ch)-1
#define SS(ch) ch, sizeof(ch)

typedef struct _acl_role {
	const char *name;
	unsigned int name_length;
} acl_role;

typedef struct _acl_resource {
	char *name;
	unsigned int name_length;
	p_hash_table *accesses;
} acl_resource;

typedef struct _acl_control_level {
	const char *name;
	unsigned int name_length;
} acl_control_level;

typedef struct _acl_list {
	char *name;
	int name_length;
	int default_access;
	acl_control_level *control_levels;
	p_hash_table *resources;
	p_hash_table *access_list;
	p_hash_table *control_list;
} acl_list;

typedef struct _proto_command {
	char *value;
	unsigned int length;
	json_object *(*func)(p_hash_table *, json_object *);
} proto_command;

typedef struct _axxel_context {
	JSRuntime *runtime;
	JSContext *jsContext;
	JSObject  *global;
	p_hash_table *acls;
} axxel_context;

#endif
