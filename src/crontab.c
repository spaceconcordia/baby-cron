/*
 * Modified version of crond from http://www.busybox.net/
 *
 * */

#include "baby-cron.h"
#include "crontab.h"
#include "config.h"
#include <dirent.h>
#include "shakespeare.h"

char bb_common_bufsiz1[COMMON_BUFSIZE] ALIGNED(sizeof(long long));

// Die if we can't copy a string to freshly allocated memory.
char* FAST_FUNC xstrdup(const char *s)
{
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup(s);

	//if (t == NULL)
		//bb_error_msg_and_die(bb_msg_memory_exhausted);

	return t;
}

// Die if we can't allocate size bytes of memory.
void* FAST_FUNC xmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL && size != 0) {
	//	bb_error_msg_and_die(bb_msg_memory_exhausted);
    }
	return ptr;
}


// Die if we can't resize previously allocated memory.  (This returns a pointer
// to the new memory, which may or may not be the same as the old memory.
// It'll copy the contents to a new chunk and free the old one if necessary.)
void* FAST_FUNC xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL && size != 0) {
//		bb_error_msg_and_die(bb_msg_memory_exhausted);
    }
	return ptr;
}

// Die if we can't allocate and zero size bytes of memory.
void* FAST_FUNC xzalloc(size_t size)
{
	void *ptr = xmalloc(size);
	memset(ptr, 0, size);
	return ptr;
}

void ParseField(char *user, char *ary, int modvalue, int off,
				const char *names, char *ptr)
/* 'names' is a pointer to a set of 3-char abbreviations */
{
	char *base = ptr;
	int n1 = -1;
	int n2 = -1;

	// this can't happen due to config_read()
	/*if (base == NULL)
		return;*/

	while (1) {
		int skip = 0;

		/* Handle numeric digit or symbol or '*' */
		if (*ptr == '*') {
			n1 = 0;  /* everything will be filled */
			n2 = modvalue - 1;
			skip = 1;
			++ptr;
		} else if (isdigit(*ptr)) {
			char *endp;
			if (n1 < 0) {
				n1 = strtol(ptr, &endp, 10) + off;
			} else {
				n2 = strtol(ptr, &endp, 10) + off;
			}
			ptr = endp; /* gcc likes temp var for &endp */
			skip = 1;
		} else if (names) {
			int i;

			for (i = 0; names[i]; i += 3) {
				/* was using strncmp before... */
				if (strncasecmp(ptr, &names[i], 3) == 0) {
					ptr += 3;
					if (n1 < 0) {
						n1 = i / 3;
					} else {
						n2 = i / 3;
					}
					skip = 1;
					break;
				}
			}
		}

		/* handle optional range '-' */
		if (skip == 0) {
			goto err;
		}
		if (*ptr == '-' && n2 < 0) {
			++ptr;
			continue;
		}

		/*
		 * collapse single-value ranges, handle skipmark, and fill
		 * in the character array appropriately.
		 */
		if (n2 < 0) {
			n2 = n1;
		}
		if (*ptr == '/') {
			char *endp;
			skip = strtol(ptr + 1, &endp, 10);
			ptr = endp; /* gcc likes temp var for &endp */
		}

		/*
		 * fill array, using a failsafe is the easiest way to prevent
		 * an endless loop
		 */
		{
			int s0 = 1;
			int failsafe = 1024;

			--n1;
			do {
				n1 = (n1 + 1) % modvalue;

				if (--s0 == 0) {
					ary[n1 % modvalue] = 1;
					s0 = skip;
				}
				if (--failsafe == 0) {
					goto err;
				}
			} while (n1 != n2);
		}
		if (*ptr != ',') {
			break;
		}
		++ptr;
		n1 = -1;
		n2 = -1;
	}

	if (*ptr) {
 err:
        char msg[LOG_BUFFER_SIZE];
        sprintf(msg, "user %s: parse error at %s", user, base);
        Shakespeare::log(g_fp_log, Shakespeare::WARNING, "Baby-Cron", string(msg));

		return;
	}

	if (DebugOpt && (G.log_level <= 5)) { /* like LVL5 */
		/* can't use crondlog, it inserts '\n' */
		int i;
		for (i = 0; i < modvalue; ++i)
			fprintf(stderr, "%d", (unsigned char)ary[i]);
	}
}

static void FixDayDow(CronLine *line)
{
	unsigned i;
	int weekUsed = 0;
	int daysUsed = 0;

	for (i = 0; i < ARRAY_SIZE(line->cl_Dow); ++i) {
		if (line->cl_Dow[i] == 0) {
			weekUsed = 1;
			break;
		}
	}
	for (i = 0; i < ARRAY_SIZE(line->cl_Days); ++i) {
		if (line->cl_Days[i] == 0) {
			daysUsed = 1;
			break;
		}
	}
	if (weekUsed != daysUsed) {
		if (weekUsed)
			memset(line->cl_Days, 0, sizeof(line->cl_Days));
		else /* daysUsed */
			memset(line->cl_Dow, 0, sizeof(line->cl_Dow));
	}
}

FILE* FAST_FUNC fopen_or_warn(const char *path, const char *mode)
{
	FILE *fp = fopen(path, mode);
	if (!fp) {
        char msg[LOG_BUFFER_SIZE];
        sprintf(msg, "Can't fopen the file : %s", path);
        Shakespeare::log(g_fp_log, Shakespeare::ERROR, "Baby-Cron", string(msg));

		//errno = 0; /* why? */
	}
	return fp;
}

FILE* FAST_FUNC fopen_or_warn_stdin(const char *filename)
{
	FILE *fp = stdin;

	if (filename != bb_msg_standard_input
	 && NOT_LONE_DASH(filename)
	) {
		fp = fopen_or_warn(filename, "r");
	}
	return fp;
}

void load_crontab(const char *fileName)
{

    struct parser_t *parser;
    struct stat sbuf;
	int maxLines;
	char **tokens= (char**)malloc(sizeof(char*)*6);
#if ENABLE_FEATURE_CROND_CALL_SENDMAIL
	char *mailTo = NULL;
#endif

	parser = config_open(fileName);
	if (!parser) {
		return;
    }

    maxLines = (strcmp(fileName, "root") == 0) ? 65535 : MAXLINES;

	fstat(fileno(parser->fp), &sbuf);
    //sbuf.st_uid = 0;
    if  (sbuf.st_uid == DAEMON_UID) {
		CronFile *file = (CronFile*) xzalloc(sizeof(CronFile));
		CronLine **pline;
		int n;

		file->cf_username = xstrdup(fileName);
		pline = &file->cf_lines;

		while (1) {
			CronLine *line;

			if (!--maxLines)
				break;

			n = config_read(parser, tokens, 6, 1, "# \t", PARSE_NORMAL | PARSE_KEEP_COPY);

			if (!n)
				break;

			//if (DebugOpt)
				//crondlog(LVL5 "user:%s entry:%s", fileName, parser->data);

			/* check if line is setting MAILTO= */
			if (0 == strncmp(tokens[0], "MAILTO=", 7)) {
#if ENABLE_FEATURE_CROND_CALL_SENDMAIL
				free(mailTo);
				mailTo = (tokens[0][7]) ? xstrdup(&tokens[0][7]) : NULL;
#endif /* otherwise just ignore such lines */
				continue;
			}
			 // check if a minimum of tokens is specified
			if (n < 6)
				continue;
			*pline = line = (CronLine*)xzalloc(sizeof(*line));
			/* parse date ranges */
			ParseField(file->cf_username, line->cl_Mins, 60, 0, NULL, tokens[0]);
			ParseField(file->cf_username, line->cl_Hrs, 24, 0, NULL, tokens[1]);
			ParseField(file->cf_username, line->cl_Days, 32, 0, NULL, tokens[2]);
			ParseField(file->cf_username, line->cl_Mons, 12, -1, MonAry, tokens[3]);
			ParseField(file->cf_username, line->cl_Dow, 7, 0, DowAry, tokens[4]);
			/*
			 * fix days and dow - if one is not "*" and the other
			 * is "*", the other is set to 0, and vise-versa
			 */
			FixDayDow(line);
#if ENABLE_FEATURE_CROND_CALL_SENDMAIL
			/* copy mailto (can be NULL) */
			line->cl_mailto = xstrdup(mailTo);
#endif
			/* copy command */
			line->cl_cmd = xstrdup(tokens[5]);
            line->cl_failures = 0;

            #ifdef CS1_DEBUG
                printf("[%s:%s] cmd : %s\n", __FILE__, __func__, line->cl_cmd); fflush(stdout);
            #endif

			pline = &line->cl_next;
//bb_error_msg("M[%s]F[%s][%s][%s][%s][%s][%s]", mailTo, tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
		}
		*pline = NULL;

		file->cf_next = G.cron_files;
		G.cron_files = file;

		/*if (maxLines == 0) {
			crondlog(WARN9 "user %s: too many lines", fileName);
		}*/
	}

	config_close(parser);
}

void rescan_crontab_dir(void)
{

	/* Re-chdir, in case directory was renamed & deleted */
	if (chdir(G.crontab_dir_name) < 0) {
        char msg[LOG_BUFFER_SIZE];
        sprintf(msg, "chdir(%s)", G.crontab_dir_name);
        Shakespeare::log(g_fp_log, Shakespeare::ERROR, "Baby-Cron", string(msg));
	}

	/* Scan directory and add associated users */
	{
		DIR *dir = opendir(".");
		struct dirent *den;

        if (!dir){
            char msg[LOG_BUFFER_SIZE];
            sprintf(msg, "chdir(%s)", ".");
            Shakespeare::log(g_fp_log, Shakespeare::ERROR, "Baby-Cron", string(msg));
            exit(-1);
        }

		while ((den = readdir(dir)) != NULL) {
			if (strchr(den->d_name, '.') != NULL) {
				continue;
			}

			load_crontab(den->d_name);

            #ifdef CS1_DEBUG
                printf("[baby-cron] crontab loaded : %s\n", den->d_name);fflush(stdout);
            #endif
		}

		closedir(dir);
	}
}

