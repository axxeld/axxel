
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

json_object *p_addrole(p_hash_table *acl_lists, json_object *params);
json_object *p_isrole(p_hash_table *acl_lists, json_object *params);
json_object *p_getroles(p_hash_table *acl_lists, json_object *params);
json_object *p_delrole(p_hash_table *acl_lists, json_object *params);