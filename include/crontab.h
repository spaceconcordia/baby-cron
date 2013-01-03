#ifndef CRONTAB_H
#define CRONTAB_H

extern "C" {
    #include "platform.h"
}

void ParseField(char *user, char *ary, int modvalue, int off, const char *names, char *ptr);

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


#endif
