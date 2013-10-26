
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

json_object *parse_proto(JSContext *cx, p_hash_table *acl_lists, char *cmd, size_t n);
JSContext *init_proto();