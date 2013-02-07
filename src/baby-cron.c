#include <errno.h>
#include "baby-cron.h"
#include "crontab.h"
#include <time.h>
#include <signal.h>
#include <sys/types.h>

unsigned long g_rtries = 1;

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

            if (line->cl_failures >= 5) {
                //TODO: Remove magic number
                line->cl_pid = 0;
                continue;
            }

			start_one_job(file->cf_username, line);
			pid = line->cl_pid;
            line->cl_time_started = time(NULL);
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

static void process_finished_job(const char *user, CronLine *line)
{
	pid_t pid;
	int mailFd;
	char mailFile[128];
	struct stat sbuf;

	pid = line->cl_pid;
	line->cl_pid = 0;
	if (pid <= 0) {
		/* No job */
		return;
	}
	//TODO: Remove mail stuff
	/*if (line->cl_empty_mail_size <= 0) {
		/* End of job and no mail file, or end of sendmail job */
		return;
	//}*/

	/*
	 * End of primary job - check for mail file.
	 * If size has changed and the file is still valid, we send it.
	 
	snprintf(mailFile, sizeof(mailFile), "%s/cron.%s.%d", TMPDIR, user, (int)pid);
	mailFd = open(mailFile, O_RDONLY);
	unlink(mailFile);
	if (mailFd < 0) {
		return;
	}

	if (fstat(mailFd, &sbuf) < 0
	 || sbuf.st_uid != DAEMON_UID
	 || sbuf.st_nlink != 0
	 || sbuf.st_size == line->cl_empty_mail_size
	 || !S_ISREG(sbuf.st_mode)
	) {
		close(mailFd);
		return;
	}
	line->cl_empty_mail_size = 0; 
	*/
	/* if (line->cl_mailto) - always true if cl_empty_mail_size was nonzero */
/*		line->cl_pid = fuork_job(user, mailFd, SENDMAIL, NULL);*/
}

int check_completions(void)
{
	CronFile *file;
	CronLine *line;
	int num_still_running = 0;

	for (file = G.cron_files; file; file = file->cf_next) {
		if (!file->cf_has_running)
			continue;

		file->cf_has_running = 0;
		for (line = file->cf_lines; line; line = line->cl_next) {
			if (line->cl_pid <= 0)
				continue;

            int status;
            unsigned long rtries = g_rtries;
			int r = waitpid(line->cl_pid, &status, WNOHANG);
            
            while (rtries > 0 && r == 0) {
		    	r = waitpid(line->cl_pid, &status, WNOHANG);
                rtries -= 1;
            }

			if (r < 0 || r == line->cl_pid) {
                if (WIFEXITED(status)) {
                    printf("exit");
                    if (WEXITSTATUS(status) == 0) {
                            printf("job finished?");
                        process_finished_job(file->cf_username, line);
                        if (line->cl_pid == 0) {
                            /* sendmail was not started for it */
                            continue;
                        }
                    /* else: sendmail was started, job is still running, fall thru */
                    }
                    else {
                        line->cl_failures += 1;
                        if (line-> cl_failures > 5) {
                            // TODO: Remove magic number
                                printf("remove cos of failure");
                            line->cl_pid = 0;
                            continue;
                        }
                        else {
                            printf("failure detected");
                            line->cl_pid = -1;
				            file->cf_wants_starting = 1;
                            continue; 
                        }
                    }
                }
                else { //crashed
line->cl_failures += 1;
                        if (line-> cl_failures > 5) {
                            // TODO: Remove magic number
                            line->cl_pid = 0;
                            continue;
                        }
                        else {
                            printf("failure detected");
                            line->cl_pid = -1;
				            file->cf_wants_starting = 1;
                            continue; 
                        }
                }
			}

            printf("in the else");
			/* else: r == 0: "process is still running" */
            if (time(NULL) - line->cl_time_started > MAX_RUN_TIME_IN_SEC) {
                kill(line->cl_pid, SIGKILL);
                line->cl_failures += 1;
                if (line-> cl_failures > 5) {
                    // TODO: Remove magic number
                    line->cl_pid = 0;
                }
                else {
                    line->cl_pid = -1;
				    file->cf_wants_starting = 1;
                }
                continue;
            }

			file->cf_has_running = 1;
		}
//FIXME: if !file->cf_has_running && file->deleted: delete it!
//otherwise deleted entries will stay forever, right?
		num_still_running += file->cf_has_running;
	}
	return num_still_running;
}

//This function exists because we needed a way to add delay to waitpid
//during integration test.
void set_rtries_for_integration_tests(int rtries) {
    g_rtries = rtries;
}
