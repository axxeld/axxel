
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

#include "stdio.h"
#include "string.h"
#include "json-c/json.h"

#include "hash.h"

#include "axxel.h"
#include "memory.h"
#include "hash.h"
#include "response.h"

char *p_getthreekey(char *role_key, unsigned int role_length, char *resource_key, unsigned int resource_length, char *access_key, unsigned int access_length, unsigned int *length) {

	char *key;

	*length = role_length + resource_length + access_length + 3;
	key = pmalloc(sizeof(char) * (*length + 1));
	memcpy(key, role_key, role_length);
	memcpy(key + role_length, "!", 1);
	memcpy(key + role_length + 1, resource_key, resource_length);
	memcpy(key + role_length + resource_length + 1, "!", 1);
	memcpy(key + role_length + resource_length + 2, access_key, access_length);
	key[*length] = '\0';

	return key;
}

acl_list *p_getacl(json_object *params) {

	const char *name;
	unsigned int name_length;
	acl_list *acl = NULL;
	json_object *acl_obj;

	acl_obj = json_object_object_get(params, "acl");
	if (!acl_obj) {
		return NULL;
	}

	if (json_object_get_type(acl_obj) != json_type_string) {
		return NULL;
	}

	name = json_object_get_string(acl_obj);
	name_length = json_object_get_string_len(acl_obj);

	if (!acl_lists) {
		acl_lists = p_hash_table_create(31, NULL);
	} else {
		acl = p_hash_table_get(acl_lists, name, name_length);
	}

	if (!acl) {

		acl = pmalloc(sizeof(acl_list));
		acl->name = name;
		acl->name_length = name_length;
		acl->roles = NULL;
		acl->resources = NULL;
		acl->access_list = NULL;
		acl->control_list = NULL;

		p_hash_table_insert(acl_lists, name, name_length, acl);
	}

	return acl;
}

json_object *p_defaultaction(json_object *params){

	acl_list *acl;
	json_object *name_obj, *value_obj;
	const char *value;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed();
	}

	value_obj = json_object_object_get(params, "value");
	if (!value_obj) {
		return p_response_failed_ex("Parameter 'value' is required");
	}

	if (json_object_get_type(value_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'value' must be a string");
	}

	value = json_object_get_string(value_obj);

	if (!memcmp(value, SS("allow"))) {
		acl->default_access = 1;
	} else {
		if (!memcmp(value, SS("deny"))) {
			acl->default_access = 0;
		} else {
			return p_response_failed_ex("Parameter 'value' must 'allow' or 'deny");
		}
	}

	return p_response_ok();
}

json_object *p_allow_or_deny(json_object *params, int action) {

	acl_list *acl;
	acl_resource *resource;
	acl_role *role;
	json_object *resource_obj, *accesses_obj, *access_obj, *role_obj;
	const char *role_name, *resource_name, *access_name;
	unsigned int access_name_length, role_name_length, resource_name_length;
	unsigned int access_type, type, array_length, i, *paction;
	p_hash_table *role_hash, *resource_hash;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	if (!acl->roles) {
		return p_response_failed_ex("Role does not exist");
	}

	role_obj = json_object_object_get(params, "role");
	if (!role_obj) {
		return p_response_failed_ex("Parameter 'role' is required");
	}

	if (json_object_get_type(role_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'role' does not have a valid type");
	}

	role_name = json_object_get_string(role_obj);
	role_name_length = json_object_get_string_len(role_obj);

	role = p_hash_table_get(acl->roles, role_name, role_name_length);
	if (!role) {
		return p_response_failed_ex("Role does not exist");
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
		if (type == json_type_string) {
			return p_response_failed_ex("Access does not exist in the resource");
		} else {
			return p_response_failed_ex("Accesses does not exist in the resource");
		}
	}

	/**
	 * Round 1: Check first that every element exist as an access in the respective resource
	 */
	if (type == json_type_string) {

		access_name = json_object_get_string(accesses_obj);
		access_name_length = json_object_get_string_len(accesses_obj);

		if (!p_hash_table_exists(resource->accesses, access_name, access_name_length)) {
			return p_response_failed_ex("Access does not exist in the resource");
		}

	} else {

		array_length = json_object_array_length(accesses_obj);

		for (i = 0; i < array_length; i++){

			access_obj = json_object_array_get_idx(accesses_obj, i);

			access_type = json_object_get_type(access_obj);
			if (access_type != json_type_string) {
				return p_response_failed_ex("All accesses must be strings");
			}

			access_name = json_object_get_string(access_obj);
			access_name_length = json_object_get_string_len(access_obj);

			if (!p_hash_table_exists(resource->accesses, access_name, access_name_length)) {
				return p_response_failed_ex("Access does not exist in the resource");
			}
		}
	}

	if (!acl->access_list) {
		acl->access_list = p_hash_table_create(31, NULL);
	}

	/**
	 * First level is the role hash
	 */
	role_hash = p_hash_table_get(acl->access_list, role_name, role_name_length);
	if (!role_hash) {
		role_hash = p_hash_table_create(31, NULL);
		p_hash_table_insert(acl->access_list, role_name, role_name_length, role_hash);
	}

	/**
	 * Second level is the resource hash
	 */
	resource_hash = p_hash_table_get(role_hash, resource_name, resource_name_length);
	if (!resource_hash) {
		resource_hash = p_hash_table_create(229, NULL);
		p_hash_table_insert(role_hash, resource_name, resource_name_length, resource_hash);
	}

	/**
	 * Round 2: Insert the permissions
	 */
	if (type == json_type_string) {

		paction = pmalloc(sizeof(unsigned int));
		memcpy(paction, &action, sizeof(unsigned int));
		p_hash_table_insert(resource_hash, access_name, access_name_length, paction);

	} else {

		array_length = json_object_array_length(accesses_obj);

		for (i = 0; i < array_length; i++){

			access_obj = json_object_array_get_idx(accesses_obj, i);

			access_name = json_object_get_string(access_obj);
			access_name_length = json_object_get_string_len(access_obj);

			paction = pmalloc(sizeof(unsigned int));
			memcpy(paction, &action, sizeof(unsigned int));
			p_hash_table_insert(resource_hash, access_name, access_name_length, paction);
		}
	}

	/**
	 * Round 3: Rebuild the ACL list
	 */
	p_rebuildlist(acl);

	return p_response_ok();
}

json_object *p_allow(json_object *params) {
	return p_allow_or_deny(params, 1);
}

json_object *p_deny(json_object *params) {
	return p_allow_or_deny(params, 0);
}

/**
 * Returns the acceses assigned to a role/resource
 */
json_object *p_getaccesses(json_object *params) {

	acl_list *acl;
	acl_resource *resource;
	acl_role *role;
	json_object *resource_obj, *response, *accesses, *role_obj, *role_accesses;
	const char *role_name, *resource_name, *access_name;
	unsigned int access_name_length, role_name_length, resource_name_length;
	unsigned int type, array_length, i, j;
	p_hash_table *role_hash, *resource_hash;
	struct p_hash_node *role_node, *role_next, *resource_node, *resource_next;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_failed_ex("ACL cannot be obtained");
	}

	if (!acl->roles) {
		return p_response_failed_ex("Role does not exist");
	}

	role_obj = json_object_object_get(params, "role");
	if (!role_obj) {
		return p_response_failed_ex("Parameter 'role' is required");
	}

	if (json_object_get_type(role_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'role' does not have a valid type");
	}

	role_name = json_object_get_string(role_obj);
	role_name_length = json_object_get_string_len(role_obj);

	role = p_hash_table_get(acl->roles, role_name, role_name_length);
	if (!role) {
		return p_response_failed_ex("Role does not exist");
	}

	resource_obj = json_object_object_get(params, "resource");
	if (resource_obj) {

		if (json_object_get_type(resource_obj) != json_type_string) {
			return p_response_failed_ex("Parameter 'resource' does not have a valid type");
		}

		resource_name = json_object_get_string(resource_obj);
		resource_name_length = json_object_get_string_len(resource_obj);

		if (!acl->resources) {
			return p_response_failed_ex("Resource does not exist");
		}

		resource = p_hash_table_get(acl->resources, resource_name, resource_name_length);
		if (!resource) {
			return p_response_failed_ex("Resource does not exist");
		}

	}

	response = json_object_new_object();

	json_object_object_add(response, "response", json_object_new_string("ok"));

	if (!acl->access_list) {
		accesses = json_object_new_object();
		json_object_object_add(response, "accesses", accesses);
		return response;
	}

	/**
	 * First level is the role hash
	 */
	role_hash = p_hash_table_get(acl->access_list, role_name, role_name_length);
	if (!role_hash) {
		accesses = json_object_new_object();
		json_object_object_add(response, "accesses", accesses);
		return response;
	}

	if (!resource_obj) {

		accesses = json_object_new_object();

		for (i = 0; i < role_hash->size; ++i) {
			for (role_node = role_hash->nodes[i]; role_node; role_node = role_next) {

				role_accesses = json_object_new_object();

				for (j = 0; j < ((p_hash_table *) role_node->data)->size; ++j) {
					for (resource_node = ((p_hash_table *) role_node->data)->nodes[j]; resource_node; resource_node = resource_next) {
						if (*((unsigned int *)resource_node->data)) {
							json_object_object_add(role_accesses, resource_node->key, json_object_new_string("allowed"));
						} else {
							json_object_object_add(role_accesses, resource_node->key, json_object_new_string("denied"));
						}
						resource_next = resource_node->next;
					}
				}

				json_object_object_add(accesses, role_node->key, role_accesses);

				role_next = role_node->next;
			}
		}

	} else {

		resource_hash = p_hash_table_get(role_hash, resource_name, resource_name_length);
		if (!resource_hash) {
			accesses = json_object_new_object();
			json_object_object_add(response, "accesses", accesses);
			return response;
		}

		accesses = json_object_new_object();

		for (j = 0; j < resource_hash->size; ++j) {
			for (resource_node = resource_hash->nodes[j]; resource_node; resource_node = resource_next) {
				if (*((unsigned int *)resource_node->data)) {
					json_object_object_add(accesses, resource_node->key, json_object_new_string("allowed"));
				} else {
					json_object_object_add(accesses, resource_node->key, json_object_new_string("denied"));
				}
				resource_next = resource_node->next;
			}
		}

	}

	json_object_object_add(response, "accesses", accesses);
	return response;
}

int p_rebuildlist(acl_list *acl) {

	char *key;
	unsigned int i, j, k, *paction, length;
	unsigned long hash;
	struct p_hash_node *role_node, *role_next, *resource_node, *resource_next;
	struct p_hash_node *resource_access_node, *resource_access_next;
	p_hash_table *role_hash, *resource_hash, *access_list;

	if (!acl->access_list) {
		return 0;
	}

	if (acl->control_list) {
		p_hash_table_destroy(acl->control_list);
	}

	access_list = p_hash_table_create(1039, NULL);

	for (i = 0; i < acl->access_list->size; ++i) {
		for (role_node = acl->access_list->nodes[i]; role_node; role_node = role_next) {

			role_hash = (p_hash_table*) role_node->data;

			for (j = 0; j < role_hash->size; ++j) {
				for (resource_node = role_hash->nodes[j]; resource_node; resource_node = resource_next) {

					resource_hash = (p_hash_table*) resource_node->data;

					for (k = 0; k < resource_hash->size; ++k) {
						for (resource_access_node = resource_hash->nodes[k]; resource_access_node; resource_access_node = resource_access_next) {

							key = p_getthreekey(
								role_node->key, role_node->key_length,
								resource_node->key, resource_node->key_length,
								resource_access_node->key, resource_access_node->key_length,
								&length
							);

							paction = pmalloc(sizeof(unsigned int));
							memcpy(paction, (unsigned int *) resource_access_node->data, sizeof(unsigned int));
							p_hash_table_insert(access_list, key, length, paction);

							resource_access_next = resource_access_node->next;
						}
					}
					resource_next = resource_node->next;
				}
			}

			role_next = role_node->next;
		}
	}

	for (i = 0; i < access_list->size; ++i) {
		for (role_node = access_list->nodes[i]; role_node; role_node = role_next) {
			fprintf(stderr, "%s\n", role_node->key);
			role_next = role_node->next;
		}
	}

	acl->control_list = access_list;

	return 1;
}

/**
 * Check if role is allowed to access a resource/access combination
 */
json_object *p_isallowed(json_object *params) {

	acl_list *acl;
	acl_resource *resource;
	acl_role *role;
	json_object *resource_obj, *role_obj, *access_obj;
	const char *role_name, *resource_name, *access_name;
	unsigned int access_name_length, role_name_length, resource_name_length, length, *paction;
	char *key;

	acl = p_getacl(params);
	if (!acl) {
		return p_response_no();
	}

	if (!acl->control_list) {
		return p_response_no();
	}

	if (!acl->roles) {
		return p_response_no();
	}

	role_obj = json_object_object_get(params, "role");
	if (!role_obj) {
		return p_response_failed_ex("Parameter 'role' is required");
	}

	if (json_object_get_type(role_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'role' does not have a valid type");
	}

	role_name = json_object_get_string(role_obj);
	role_name_length = json_object_get_string_len(role_obj);

	role = p_hash_table_get(acl->roles, role_name, role_name_length);
	if (!role) {
		return p_response_failed_ex("Role does not exist");
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

	if (!acl->resources) {
		return p_response_failed_ex("Resource does not exist");
	}

	resource = p_hash_table_get(acl->resources, resource_name, resource_name_length);
	if (!resource) {
		return p_response_failed_ex("Resource does not exist");
	}

	access_obj = json_object_object_get(params, "access");
	if (!access_obj) {
		return p_response_failed_ex("Parameter 'access' is required");
	}

	if (json_object_get_type(access_obj) != json_type_string) {
		return p_response_failed_ex("Parameter 'access' does not have a valid type");
	}

	access_name = json_object_get_string(access_obj);
	access_name_length = json_object_get_string_len(access_obj);

	key = p_getthreekey(
		(char *) role_name, role_name_length,
		(char *) resource_name, resource_name_length,
		(char *) access_name, access_name_length,
		&length
	);

	paction = p_hash_table_get(acl->control_list, key, length);
	if (paction) {
		if (*paction) {
			return p_response_yes();
		}
	}

	return p_response_no();
}
