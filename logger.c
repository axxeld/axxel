
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

void p_log(char *message, int severity) {
	fprintf(stderr, "%s\n", message);
}