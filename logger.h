
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

#define AXXEL_LOGGING 1

#define AXXEL_LOG_DEBUG 0
#define AXXEL_LOG_NOTICE 1
#define AXXEL_LOG_INFO 2
#define AXXEL_LOG_WARNING 3
#define AXXEL_LOG_ERROR 4

#define p_log_debug(msg) if (AXXEL_LOGGING) p_log(msg, AXXEL_LOG_DEBUG)
#define p_log_notice(msg) if (AXXEL_LOGGING) p_log(msg, AXXEL_LOG_NOTICE)
#define p_log_warning(msg) if (AXXEL_LOGGING) p_log(msg, AXXEL_LOG_WARNING)
#define p_log_error(msg) if (AXXEL_LOGGING) p_log(msg, AXXEL_LOG_ERROR)

void p_log(char *message, int severity);