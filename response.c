
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

json_object *p_response_ok() {

	json_object *status;

	status = json_object_new_object();

	json_object_object_add(status, "response", json_object_new_string("ok"));

	return status;
}

json_object *p_response_failed() {

	json_object *status;

	status = json_object_new_object();

	json_object_object_add(status, "response", json_object_new_string("failed"));

	return status;
}

json_object *p_response_yes() {

	json_object *status;

	status = json_object_new_object();

	json_object_object_add(status, "response", json_object_new_string("yes"));

	return status;
}

json_object *p_response_no() {

	json_object *status;

	status = json_object_new_object();

	json_object_object_add(status, "response", json_object_new_string("no"));

	return status;
}

json_object *p_response_failed_ex(const char *description) {

	json_object *status;

	status = json_object_new_object();

	json_object_object_add(status, "response", json_object_new_string("failed"));
	json_object_object_add(status, "description", json_object_new_string(description));

	return status;
}
