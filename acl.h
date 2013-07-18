
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

#define ACTION_ALLOW 1
#define ACTION_DENY 0

acl_list *p_getacl(p_hash_table *acl_lists, json_object *params);

json_object *p_defaultaction(p_hash_table *acl_lists, json_object *params);
json_object *p_createacl(p_hash_table *acl_lists, json_object *params);
json_object *p_allow(p_hash_table *acl_lists, json_object *params);
json_object *p_deny(p_hash_table *acl_lists, json_object *params);
json_object *p_getaccesses(p_hash_table *acl_lists, json_object *params);
json_object *p_rebuildlist(acl_list *acl);
json_object *p_isallowed(p_hash_table *acl_lists, json_object *params);