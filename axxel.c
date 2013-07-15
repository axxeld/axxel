
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

#include "json/json.h"

#include "hash.h"

#include "axxel.h"
#include "protocol.h"

p_hash_table *acl_lists = NULL;

int main(int argc, char **argv) {
	start_server();
	return 0;
}

