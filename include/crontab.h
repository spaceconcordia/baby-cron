#ifndef CRONTAB_H
#define CRONTAB_H

#include <sys/stat.h>
#include <stdio.h> //For BUFSIZ

extern "C" {
    #include "platform.h"

}

#ifndef MAXLINES
# define MAXLINES       256  /* max lines in non-root crontabs */
#endif

#define DAEMON_UID 0

#define NOT_LONE_DASH(s) ((s)[0] != '-' || (s)[1])

const char bb_msg_standard_input[] ALIGN1 = "standard input";

typedef struct CronFile {
	struct CronFile *cf_next;
	struct CronLine *cf_lines;
	char *cf_username;
	smallint cf_wants_starting;     /* bool: one or more jobs ready */
	smallint cf_has_running;        /* bool: one or more jobs running */
	smallint cf_deleted;            /* marked for deletion (but still has running jobs) */
} CronFile;

typedef struct CronLine {
	struct CronLine *cl_next;
	char *cl_cmd;                   /* shell command */
	pid_t cl_pid;                   /* >0:running, <0:needs to be started in this minute, 0:dormant */
    int cl_failures;                   /* Number of times exited with a failure */
#if ENABLE_FEATURE_CROND_CALL_SENDMAIL
	int cl_empty_mail_size;         /* size of mail header only, 0 if no mailfile */
	char *cl_mailto;                /* whom to mail results, may be NULL */
#endif
	/* ordered by size, not in natural order. makes code smaller: */
	char cl_Dow[7];                 /* 0-6, beginning sunday */
	char cl_Mons[12];               /* 0-11 */
	char cl_Hrs[24];                /* 0-23 */
	char cl_Days[32];               /* 1-31 */
	char cl_Mins[60];               /* 0-59 */
} CronLine;

enum { COMMON_BUFSIZE = (BUFSIZ >= 256*sizeof(void*) ? BUFSIZ+1 : 256*sizeof(void*)) };
extern char bb_common_bufsiz1[COMMON_BUFSIZE]; //Defined in crontab.c because it was causing definition clashes in the test file

#define CRONTABS "/crontabs"

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
    G.cron_files = NULL; \
} while (0)

/* 0 is the most verbose, default 8 */
#define LVL5  "\x05"
#define LVL7  "\x07"
#define LVL8  "\x08"
#define WARN9 "\x49"
#define DIE9  "\xc9"
/* level >= 20 is "error" */
#define ERR20 "\x14"

static const char DowAry[] ALIGN1 =
	"sun""mon""tue""wed""thu""fri""sat"
;

static const char MonAry[] ALIGN1 =
	"jan""feb""mar""apr""may""jun""jul""aug""sep""oct""nov""dec"
;

void ParseField(char *user, char *ary, int modvalue, int off, const char *names, char *ptr);
void load_crontab(const char *filename);
void rescan_crontab_dir(void);

#endif
