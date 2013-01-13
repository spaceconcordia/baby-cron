#include "baby-cron.h"
#include "crontab.h"

#if SETENV_LEAKS
/* We set environment *before* vfork (because we want to use vfork),
 * so we cannot use setenv() - repeated calls to setenv() may leak memory!
 * Using putenv(), and freeing memory after unsetenv() won't leak */
static void safe_setenv(char **pvar_val, const char *var, const char *val)
{
	char *var_val = *pvar_val;

	if (var_val) {
		bb_unsetenv_and_free(var_val);
	}
	*pvar_val = sprintf("%s=%s", var, val); //was xasprintf before
	putenv(*pvar_val);
}
#endif

static void set_env_vars(struct passwd *pas)
{
#if SETENV_LEAKS
	safe_setenv(&G.env_var_user, "USER", pas->pw_name);
	safe_setenv(&G.env_var_home, "HOME", pas->pw_dir);
	/* if we want to set user's shell instead: */
	/*safe_setenv(G.env_var_shell, "SHELL", pas->pw_shell);*/
#else
	//TODO: investigate this
	//xsetenv("USER", pas->pw_name);
    //xsetenv("HOME", pas->pw_dir);
#endif
	/* currently, we use constant one: */
	/*setenv("SHELL", DEFAULT_SHELL, 1); - done earlier */
}

/*
 * Determine which jobs need to be run.  Under normal conditions, the
 * period is about a minute (one scan).  Worst case it will be one
 * hour (60 scans).
 */
void flag_starting_jobs(time_t t1, time_t t2)
{
	time_t t;

	/* Find jobs > t1 and <= t2 */

	for (t = t1 - t1 % 60; t <= t2; t += 60) {
		struct tm *ptm;
		CronFile *file;
		CronLine *line;

		if (t <= t1)
			continue;

		ptm = localtime(&t);
		for (file = G.cron_files; file; file = file->cf_next) {
			//if (DebugOpt)
				///crondlog(LVL5 "file %s:", file->cf_username);
			if (file->cf_deleted)
				continue;
			for (line = file->cf_lines; line; line = line->cl_next) {
//				if (DebugOpt)	
//					crondlog(LVL5 " line %s", line->cl_cmd);
				if (line->cl_Mins[ptm->tm_min]
				 && line->cl_Hrs[ptm->tm_hour]
				 && (line->cl_Days[ptm->tm_mday] || line->cl_Dow[ptm->tm_wday])
				 && line->cl_Mons[ptm->tm_mon]
				) {
					if (DebugOpt) {
						/*crondlog(LVL5 " job: %d %s",
							(int)line->cl_pid, line->cl_cmd);*/
                        //TODO: use shakespeare
					}
					if (line->cl_pid > 0) {
                        /*
						crondlog(LVL8 "user %s: process already running: %s",
							file->cf_username, line->cl_cmd);
                        */
                        //TODO: use shakespeare
					} else if (line->cl_pid == 0) {
						line->cl_pid = -1;
						file->cf_wants_starting = 1;
				    }	
				}
			}
		}
	}
}

void start_one_job(const char *user, CronLine *line)
{
	struct passwd *pas;
	pid_t pid;

	pas = getpwnam(user);
	if (!pas) {
		// crondlog(WARN9 "can't get uid for %s", user); TODO: use shakespeare
		goto err;
	}

	/* Prepare things before vfork */
	set_env_vars(pas);

	/* Fork as the user in question and run program */
	pid = vfork();
	if (pid == 0) {
		/* CHILD */
		/* initgroups, setgid, setuid, and chdir to home or TMPDIR */
		//change_user(pas);
		/*if (DebugOpt) {
			crondlog(LVL5 "child running %s", DEFAULT_SHELL);
		}*/
		/* crond 3.0pl1-100 puts tasks in separate process groups */
		bb_setpgrp();
		execl(DEFAULT_SHELL, DEFAULT_SHELL, "-c", line->cl_cmd, (char *) NULL);
//		crondlog(ERR20 "can't execute '%s' for user %s", DEFAULT_SHELL, user); TODO: use shakespeare
		_exit(EXIT_SUCCESS);
	}
	if (pid < 0) {
		/* FORK FAILED */
		//crondlog(ERR20 "can't vfork"); TODO: use shakespeare
 err:
		pid = 0;
	}
	line->cl_pid = pid;
}

void start_jobs(void)
{
	CronFile *file;
	CronLine *line;

	for (file = G.cron_files; file; file = file->cf_next) {
		if (!file->cf_wants_starting)
			continue;

		file->cf_wants_starting = 0;
		for (line = file->cf_lines; line; line = line->cl_next) {
			pid_t pid;
			if (line->cl_pid >= 0)
				continue;

			start_one_job(file->cf_username, line);
			pid = line->cl_pid;
			// TODO: use shakespeare crondlog(LVL8 "USER %s pid %3d cmd %s",
				//file->cf_username, (int)pid, line->cl_cmd);
			if (pid < 0) {
				file->cf_wants_starting = 1;
			}
			if (pid > 0) {
				file->cf_has_running = 1;
			}
		}
	}
}

