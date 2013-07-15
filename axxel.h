
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

#define SL(ch) ch, sizeof(ch)-1
#define SS(ch) ch, sizeof(ch)

typedef struct _acl_role {
	const char *name;
	unsigned int name_length;
} acl_role;

typedef struct _acl_resource {
	const char *name;
	unsigned int name_length;
	p_hash_table *accesses;
} acl_resource;

typedef struct _acl_list {
	const char *name;
	int name_length;
	int default_access;
	p_hash_table *roles;
	p_hash_table *resources;
	p_hash_table *access_list;
	p_hash_table *control_list;
} acl_list;

typedef struct _command {
	char *value;
	unsigned int length;
	json_object *(*func)(json_object *params);
} command;

extern p_hash_table *acl_lists;
