
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

json_object *p_addresource(p_hash_table *acl_lists, json_object *params);
json_object *p_isresource(p_hash_table *acl_lists, json_object *params);
json_object *p_getresources(p_hash_table *acl_lists, json_object *params);
json_object *p_countresources(p_hash_table *acl_lists, json_object *params);
json_object *p_delresource(p_hash_table *acl_lists, json_object *params);
json_object *p_addresourceaccess(p_hash_table *acl_lists, json_object *params);
json_object *p_isresourceaccess(p_hash_table *acl_lists, json_object *params);
json_object *p_getresourceaccesses(p_hash_table *acl_lists, json_object *params);
json_object *p_delresourceaccesses(p_hash_table *acl_lists, json_object *params);