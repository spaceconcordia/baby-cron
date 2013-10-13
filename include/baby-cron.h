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
#include "crontab.h"

#define MAX_RUN_TIME_IN_SEC 30
#define MAX_FAILURES 5
#define DEFAULT_SHELL "/bin/sh"

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

/* glibc frees previous setenv'ed value when we do next setenv()
 * of the same variable. uclibc does not do this! */
#if (defined(__GLIBC__) && !defined(__UCLIBC__)) /* || OTHER_SAFE_LIBC... */
# define SETENV_LEAKS 0
#else
# define SETENV_LEAKS 1
#endif

static FILE* g_fp_log;

#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))
void* xmalloc(size_t size) FAST_FUNC RETURNS_MALLOC;
void *xzalloc(size_t size) FAST_FUNC RETURNS_MALLOC;
void *xrealloc(void *old, size_t size) FAST_FUNC;
char *xstrdup(const char *s) FAST_FUNC RETURNS_MALLOC;

FILE* fopen_or_warn(const char *filename, const char *mode) FAST_FUNC;
FILE* fopen_or_warn_stdin(const char *filename) FAST_FUNC;

void flag_starting_jobs(time_t t1, time_t t2);
void start_one_job(const char *user, CronLine *line);
void start_jobs(void);
int check_completions(void);
void set_rtries_for_integration_tests(int rtries);
#endif
