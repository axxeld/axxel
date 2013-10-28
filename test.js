
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

load("axxel.js")
load("acl.js")
load("roles.js")
load("resources.js")

print(axxel.getAcl('my-acl').allowed('Administrators', 'Customers', ["update", "insert"]));
