
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

 #include "stdlib.h"

#include "json/json.h"
#include "hash.h"

#include "axxel.h"
#include "acl.h"
#include "response.h"

json_object *p_addrole(json_object *params){

	acl_list *acl;
	acl_role *role = NULL;
	json_object *name_obj;
	const char *name;
	unsigned int name_length;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	name_obj = json_object_object_get(params, "name");
	if (!name_obj) {
		return p_response_failed_ex("Parameter 'name' is required");
	}

	if (json_object_get_type(name_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'name' does not have a valid type");
	}

	name = json_object_get_string(name_obj);
	name_length = json_object_get_string_len(name_obj);

	if (!acl->roles) {
		acl->roles = p_hash_table_create(17, NULL);
	} else {
		role = p_hash_table_get(acl->roles, name, name_length);
	}

	if (!role) {

		role = malloc(sizeof(acl_role));
		role->name = name;
		role->name_length = name_length;
		p_hash_table_insert(acl->roles, name, name_length, role);

		return p_response_ok();
	}

	return p_response_failed();
}

json_object *p_isrole(json_object *params){

	acl_list *acl;
	acl_role *role;
	json_object *name_obj;
	const char *name;
	unsigned int name_length;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed();
	}

	name_obj = json_object_object_get(params, "name");
	if (!name_obj) {
		return p_response_failed_ex("Parameter 'name' is required");
	}

	if (json_object_get_type(name_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'name' is not valid");
	}

	if (!acl->roles) {
		return p_response_no();
	}

	name = json_object_get_string(name_obj);
	name_length = json_object_get_string_len(name_obj);

	role = p_hash_table_get(acl->roles, name, name_length);
	if (!role) {
		return p_response_no();
	}

	return p_response_yes();
}

json_object *p_getroles(json_object *params){

	unsigned int n, length;
	acl_list *acl;
	acl_role *role;
	struct p_hash_node *node, *next;
	json_object *response, *roles;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed();
	}

	if (acl->roles == NULL) {
		return p_response_failed();
	}

	response = json_object_new_object();

	json_object_object_add(response, "response", json_object_new_string("ok"));

	roles = json_object_new_array();

	for (n = 0; n < acl->roles->size; ++n) {
		for (node = acl->roles->nodes[n]; node; node = next) {
			role = (acl_role *)(node->data);
			json_object_array_add(roles, json_object_new_string(role->name));
			next = node->next;
		}
	}

	json_object_object_add(response, "roles", roles);

	return response;
}

json_object *p_delrole(json_object *params){

	acl_list *acl;
	json_object *name_obj;
	const char *name;
	unsigned int name_length;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed();
	}

	name_obj = json_object_object_get(params, "name");
	if (!name_obj) {
		return p_response_failed();
	}

	if (json_object_get_type(name_obj) != json_type_string) {
		return p_response_failed();
	}

	name = json_object_get_string(name_obj);
	name_length = json_object_get_string_len(name_obj);

	if (p_hash_table_remove(acl->roles, name, name_length)) {
		return p_response_failed();
	}

	return p_response_ok();
}
