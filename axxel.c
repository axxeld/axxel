
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

#include "json-c/json.h"

#include "hash.h"

#include "axxel.h"
#include "protocol.h"

p_hash_table *acl_lists = NULL;

int main(int argc, char **argv) {

	json_object *response;
	p_hash_table *p;

	//p_getacl(SL("hello"));
	//p_getacl(SL("hello"));

	/*response = parse_proto(SL("{'action':'add-role', 'acl':'app', 'name': 'Guests'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));
	//json_put_object(response);*/

	response = parse_proto(SL("{'action':'add-role', 'acl':'app', 'name': 'Guests'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'add-role', 'acl':'app', 'name': 'Administrators'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	/*response = parse_proto(SL("{'action':'is-role', 'acl':'app', 'name': 'Guests'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'is-role', 'acl':'app', 'name': 'Administrators'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-roles', 'acl':'app'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'add-role', 'acl':'app', 'name': 'Administrators'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-roles', 'acl':'app'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'delete-role', 'acl':'app', 'name': 'Administrators'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-roles', 'acl':'app'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));*/

	response = parse_proto(SL("{'action':'add-resource', 'acl':'app', 'name': 'Products'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'add-resource', 'acl':'app', 'name': 'Ñaçavôkù'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	/*response = parse_proto(SL("{'action':'get-resources', 'acl':'app'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));*/

	response = parse_proto(SL("{'action':'add-resource-access', 'acl':'app', 'resource': 'Products', 'accesses': 'index'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'add-resource-access', 'acl':'app', 'resource': 'Products', 'accesses': ['index', 'insert', 'update', 'search']}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	/*response = parse_proto(SL("{'action':'is-resource-access', 'acl':'app', 'resource': 'Products', 'accesses': 'index'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'is-resource-access', 'acl':'app', 'resource': 'Products', 'accesses': 'lol'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'is-resource-access', 'acl':'app', 'resource': 'Products', 'accesses': ['lol', 'index', 'search']}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-resource-accesses', 'acl':'app', 'resource': 'Products'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-resource-accesses', 'acl':'app', 'resource': 'áéíóú'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'delete-resource-access', 'acl':'app', 'resource': 'Products', 'accesses': ['lol', 'index', 'search']}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-resource-accesses', 'acl':'app', 'resource': 'Products'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'default-action', 'acl':'app', 'value': 'allow'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));*/

	response = parse_proto(SL("{'action':'allow', 'acl':'app', 'role': 'Guests', 'resource': 'Products', 'accesses': 'search'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'allow', 'acl':'app', 'role': 'Guests', 'resource': 'Products', 'accesses': ['insert', 'search']}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'deny', 'acl':'app', 'role': 'Guests', 'resource': 'Products', 'accesses': ['insert']}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'get-accesses', 'acl':'app', 'role': 'Guests'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	//response = parse_proto(SL("{'action':'get-accesses', 'acl':'app', 'role': 'Guests', 'resource': 'Products'}"));
	//fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'allowed', 'acl':'app', 'role': 'Guests', 'resource': 'Products', 'access': 'search'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'allowed', 'acl':'app', 'role': 'Guests', 'resource': 'Products', 'access': 'insert'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));

	response = parse_proto(SL("{'action':'allowed', 'acl':'app', 'role': 'Guests', 'resource': 'Products', 'access': 'insertx'}"));
	fprintf(stderr, "%s\n", json_object_to_json_string(response));
	//p = p_hash_table_create(31, NULL);
	//fprintf(stderr, "%2.4f\n", p_hash_table_empty_ratio(p));

	return 0;
}

