
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

acl_list *p_getacl(json_object *params);
json_object *p_defaultaction(json_object *params);
json_object *p_allow(json_object *params);
json_object *p_deny(json_object *params);
json_object *p_getaccesses(json_object *params);
json_object *p_rebuildlist(acl_list *acl);
json_object *p_isallowed(json_object *params);