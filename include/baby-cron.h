#ifndef BABYCRON_H
#define BABYCRON_H

#include "platform.h"
#include <syslog.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <stdarg.h> //va_start
#include <string.h> //memset
#include <ctype.h> //isdigit

#include <sys/types.h>
#include <pwd.h>

#include "config.h"

#define ENABLE_FEATURE_CROND_D 0

#define DAEMON_UID 0

enum {
	OPT_l = (1 << 0),
	OPT_L = (1 << 1),
	OPT_f = (1 << 2),
	OPT_b = (1 << 3),
	OPT_S = (1 << 4),
	OPT_c = (1 << 5),
	OPT_d = (1 << 6) * ENABLE_FEATURE_CROND_D,
};
#if ENABLE_FEATURE_CROND_D
# define DebugOpt (option_mask32 & OPT_d)
#else
# define DebugOpt 0
#endif


#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))
void* xmalloc(size_t size) FAST_FUNC RETURNS_MALLOC;
void *xzalloc(size_t size) FAST_FUNC RETURNS_MALLOC;
void *xrealloc(void *old, size_t size) FAST_FUNC;
char *xstrdup(const char *s) FAST_FUNC RETURNS_MALLOC;

FILE* fopen_or_warn(const char *filename, const char *mode) FAST_FUNC;
FILE* fopen_or_warn_stdin(const char *filename) FAST_FUNC;
#endif
