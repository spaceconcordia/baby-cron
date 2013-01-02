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

#include "crontab.h"

#define ENABLE_FEATURE_CROND_D 0

enum { COMMON_BUFSIZE = (BUFSIZ >= 256*sizeof(void*) ? BUFSIZ+1 : 256*sizeof(void*)) };
char bb_common_bufsiz1[COMMON_BUFSIZE] ALIGNED(sizeof(long long));

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


struct globals {
	unsigned log_level; /* = 8; */
	time_t crontab_dir_mtime;
	const char *log_filename;
	const char *crontab_dir_name; /* = CRONTABS; */
	CronFile *cron_files;
#if SETENV_LEAKS
	char *env_var_user;
	char *env_var_home;
#endif
} FIX_ALIASING;
#define G (*(struct globals*)&bb_common_bufsiz1)
#define INIT_G() do { \
	G.log_level = 8; \
	G.crontab_dir_name = CRONTABS; \
} while (0)


/* 0 is the most verbose, default 8 */
#define LVL5  "\x05"
#define LVL7  "\x07"
#define LVL8  "\x08"
#define WARN9 "\x49"
#define DIE9  "\xc9"
/* level >= 20 is "error" */
#define ERR20 "\x14"

#endif
