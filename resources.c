
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

#include "json-c/json.h"

#include "hash.h"

#include "axxel.h"
#include "memory.h"

#include "acl.h"
#include "response.h"

json_object *p_addresource(json_object *params){

	acl_list *acl;
	acl_resource *resource = NULL;
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

	if (!acl->resources) {
		acl->resources = p_hash_table_create(16, NULL);
	} else {
		resource = p_hash_table_get(acl->resources, name, name_length);
	}

	if (!resource) {

		resource = pmalloc(sizeof(acl_resource));
		resource->name = name;
		resource->name_length = name_length;
		resource->accesses = NULL;
		p_hash_table_insert(acl->resources, name, name_length, resource);

		return p_response_ok();
	}

	return p_response_failed();
}

json_object *p_isresource(json_object *params){

	acl_list *acl;
	acl_resource *resource;
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

	resource = p_hash_table_get(acl->resources, name, name_length);
	if (!resource) {
		return p_response_no();
	}

	return p_response_yes();
}

json_object *p_getresources(json_object *params){

	unsigned int n, length;
	acl_list *acl;
	acl_resource *resource;
	struct p_hash_node *node, *next;
	json_object *response, *resources;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	response = json_object_new_object();

	json_object_object_add(response, "response", json_object_new_string("ok"));

	resources = json_object_new_array();

	if (acl->resources != NULL) {
		for (n = 0; n < acl->resources->size; ++n) {
			for (node = acl->resources->nodes[n]; node; node = next) {
				resource = (acl_resource *)(node->data);
				json_object_array_add(resources, json_object_new_string(resource->name));
				next = node->next;
			}
		}
	}

	json_object_object_add(response, "resources", resources);

	return response;
}

/**
 * Deletes a resource
 */
json_object *p_delresource(json_object *params){

	acl_list *acl;
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

	if (p_hash_table_remove(acl->resources, name, name_length)) {
		return p_response_failed();
	}

	return p_response_ok();
}

json_object *p_addresourceaccess(json_object *params) {

	acl_list *acl;
	acl_resource *resource;
	json_object *resource_obj, *accesses_obj, *access_obj;
	const char *resource_name, *access_name;
	unsigned int access_name_length, resource_name_length, type, array_length, i, *dummy;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	if (!acl->resources) {
		return p_response_failed_ex("Resource does not exist");
	}

	resource_obj = json_object_object_get(params, "resource");
	if (!resource_obj) {
		return p_response_failed_ex("Parameter 'resource' is required");
	}

	if (json_object_get_type(resource_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'resource' does not have a valid type");
	}

	resource_name = json_object_get_string(resource_obj);
	resource_name_length = json_object_get_string_len(resource_obj);

	resource = p_hash_table_get(acl->resources, resource_name, resource_name_length);
	if (!resource) {
		return p_response_failed_ex("Resource does not exist");
	}

	accesses_obj = json_object_object_get(params, "accesses");
	if (!accesses_obj) {
		return p_response_failed_ex("Parameter 'accesses' is required");
	}

	type = json_object_get_type(accesses_obj);
	if (type != json_type_string && type != json_type_array) {
		return p_response_failed_ex("Parameter 'accesses' does not have a valid type");
	}

	if (!resource->accesses) {
		resource->accesses = p_hash_table_create(31, NULL);
	}

	if (type == json_type_string) {

		access_name = json_object_get_string(accesses_obj);
		access_name_length = json_object_get_string_len(accesses_obj);

		dummy = pmalloc(sizeof(unsigned int));
		*dummy = 1;
		p_hash_table_insert(resource->accesses, access_name, access_name_length, dummy);

	} else {

		array_length = json_object_array_length(accesses_obj);

		for (i = 0; i < array_length; i++){

			access_obj = json_object_array_get_idx(accesses_obj, i);

			type = json_object_get_type(access_obj);
			if (type != json_type_string) {
				return p_response_failed_ex("All accesses must be strings");
			}

			access_name = json_object_get_string(access_obj);
			access_name_length = json_object_get_string_len(access_obj);

			dummy = pmalloc(sizeof(unsigned int));
			*dummy = 1;
			p_hash_table_insert(resource->accesses, access_name, access_name_length, dummy);
		}
	}

	return p_response_ok();
}

json_object *p_isresourceaccess(json_object *params) {

	acl_list *acl;
	acl_resource *resource;
	void *tmp;
	json_object *resource_obj, *accesses_obj, *access_obj, *checkings, *response;
	const char *resource_name, *access_name;
	unsigned int access_name_length, resource_name_length, type, array_length, i;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	if (!acl->resources) {
		return p_response_failed_ex("Resource does not exist");
	}

	resource_obj = json_object_object_get(params, "resource");
	if (!resource_obj) {
		return p_response_failed_ex("Parameter 'resource' is required");
	}

	if (json_object_get_type(resource_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'resource' does not have a valid type");
	}

	resource_name = json_object_get_string(resource_obj);
	resource_name_length = json_object_get_string_len(resource_obj);

	resource = p_hash_table_get(acl->resources, resource_name, resource_name_length);
	if (!resource) {
		return p_response_failed_ex("Resource does not exist");
	}

	if (!resource->accesses) {
		return p_response_no();
	}

	accesses_obj = json_object_object_get(params, "accesses");
	if (!accesses_obj) {
		return p_response_failed_ex("Parameter 'accesses' is required");
	}

	type = json_object_get_type(accesses_obj);
	if (type != json_type_string && type != json_type_array) {
		return p_response_failed_ex("Parameter 'accesses' does not have a valid type");
	}

	if (type == json_type_string) {

		access_name = json_object_get_string(accesses_obj);
		access_name_length = json_object_get_string_len(accesses_obj);

		tmp = p_hash_table_get(resource->accesses, access_name, access_name_length);
		if (tmp) {
			return p_response_yes();
		}

	} else {

		response = json_object_new_object();

		json_object_object_add(response, "response", json_object_new_string("ok"));

		checkings = json_object_new_object();

		array_length = json_object_array_length(accesses_obj);

		for (i = 0; i < array_length; i++){

			access_obj = json_object_array_get_idx(accesses_obj, i);

			type = json_object_get_type(access_obj);
			if (type != json_type_string) {
				return p_response_failed_ex("All accesses must be strings");
			}

			access_name = json_object_get_string(access_obj);
			access_name_length = json_object_get_string_len(access_obj);

			tmp = p_hash_table_get(resource->accesses, access_name, access_name_length);
			if (tmp) {
				json_object_object_add(checkings, access_name, json_object_new_string("yes"));
			} else {
				json_object_object_add(checkings, access_name, json_object_new_string("no"));
			}

		}

		json_object_object_add(response, "checkings", checkings);

		return response;
	}

	return p_response_no();
}

json_object *p_getresourceaccesses(json_object *params) {

	acl_list *acl;
	acl_resource *resource;
	struct p_hash_node *node, *next;
	json_object *resource_obj, *accesses, *response;
	const char *resource_name;
	unsigned int resource_name_length, i;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	if (!acl->resources) {
		return p_response_failed_ex("Resource does not exist");
	}

	resource_obj = json_object_object_get(params, "resource");
	if (!resource_obj) {
		return p_response_failed_ex("Parameter 'resource' is required");
	}

	if (json_object_get_type(resource_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'resource' does not have a valid type");
	}

	resource_name = json_object_get_string(resource_obj);
	resource_name_length = json_object_get_string_len(resource_obj);

	resource = p_hash_table_get(acl->resources, resource_name, resource_name_length);
	if (!resource) {
		return p_response_failed_ex("Resource does not exist");
	}

	response = json_object_new_object();

	json_object_object_add(response, "response", json_object_new_string("ok"));

	accesses = json_object_new_array();

	if (resource->accesses != NULL) {
		for (i = 0; i < resource->accesses->size; ++i) {
			for (node = resource->accesses->nodes[i]; node; node = next) {
				json_object_array_add(accesses, json_object_new_string(node->key));
				next = node->next;
			}
		}
	}

	json_object_object_add(response, "accesses", accesses);

	return response;
}

json_object *p_delresourceaccesses(json_object *params) {

	acl_list *acl;
	acl_resource *resource;
	void *tmp;
	json_object *resource_obj, *accesses_obj, *access_obj, *deletes, *response;
	const char *resource_name, *access_name;
	unsigned int access_name_length, resource_name_length, type, array_length, i;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	if (!acl->resources) {
		return p_response_failed_ex("Resource does not exist");
	}

	resource_obj = json_object_object_get(params, "resource");
	if (!resource_obj) {
		return p_response_failed_ex("Parameter 'resource' is required");
	}

	if (json_object_get_type(resource_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'resource' does not have a valid type");
	}

	resource_name = json_object_get_string(resource_obj);
	resource_name_length = json_object_get_string_len(resource_obj);

	resource = p_hash_table_get(acl->resources, resource_name, resource_name_length);
	if (!resource) {
		return p_response_failed_ex("Resource does not exist");
	}

	if (!resource->accesses) {
		return p_response_no();
	}

	accesses_obj = json_object_object_get(params, "accesses");
	if (!accesses_obj) {
		return p_response_failed_ex("Parameter 'accesses' is required");
	}

	type = json_object_get_type(accesses_obj);
	if (type != json_type_string && type != json_type_array) {
		return p_response_failed_ex("Parameter 'accesses' does not have a valid type");
	}

	if (type == json_type_string) {

		access_name = json_object_get_string(accesses_obj);
		access_name_length = json_object_get_string_len(accesses_obj);

		if (p_hash_table_remove(resource->accesses, access_name, access_name_length)) {
			return p_response_yes();
		}

	} else {

		response = json_object_new_object();

		json_object_object_add(response, "response", json_object_new_string("ok"));

		deletes = json_object_new_object();

		array_length = json_object_array_length(accesses_obj);

		for (i = 0; i < array_length; i++){

			access_obj = json_object_array_get_idx(accesses_obj, i);

			type = json_object_get_type(access_obj);
			if (type != json_type_string) {
				return p_response_failed_ex("All accesses must be strings");
			}

			access_name = json_object_get_string(access_obj);
			access_name_length = json_object_get_string_len(access_obj);

			if (p_hash_table_remove(resource->accesses, access_name, access_name_length)) {
				json_object_object_add(deletes, access_name, json_object_new_string("yes"));
			} else {
				json_object_object_add(deletes, access_name, json_object_new_string("no"));
			}

		}

		json_object_object_add(response, "deletes", deletes);

		return response;
	}

	return p_response_no();
}
