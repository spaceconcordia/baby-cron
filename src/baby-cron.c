/*
 * Modified version of crond from  http://www.busybox.net/
 *
 * */

#include <errno.h>
#include "baby-cron.h"
#include "crontab.h"
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include "shakespeare.h"
#include <UpdaterClient.h>

#define PROCESS "Baby-Cron"

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
                        char msg[LOG_BUFFER_SIZE];
                        sprintf(msg, "job: %d %s", (int)line->cl_pid, line->cl_cmd);
                        Shakespeare::log(g_fp_log, Shakespeare::DEBUG, PROCESS, string(msg));
					}
					if (line->cl_pid > 0) {
                        char msg[LOG_BUFFER_SIZE];
                        sprintf(msg, "user %s: process already running: %s", file->cf_username, line->cl_cmd);
                        Shakespeare::log(g_fp_log, Shakespeare::WARNING, PROCESS, string(msg));
					} else if (line->cl_pid == 0) {
						line->cl_pid = -1;
						file->cf_wants_starting = 1;
				    }
				}
			}
		}
	}
}

static void process_finished_job(const char *user, CronLine *line)
{
	pid_t pid;
	pid = line->cl_pid;
	line->cl_pid = 0;
	if (pid <= 0) {
		/* No job */
		return;
	}
}

void start_one_job(const char *user, CronLine *line)
{
	struct passwd *pas;
	pid_t pid;


    FILE *file = fopen(line->cl_cmd, "r");
    if (file == NULL) {
        line->cl_pid = 0;
        return;
    }

    fclose(file);

	pas = getpwnam(user);
	if (!pas) {
        char msg[LOG_BUFFER_SIZE];
        sprintf(msg, "can't get uid for %s", user);
        Shakespeare::log(g_fp_log, Shakespeare::WARNING, PROCESS, string(msg));
		goto err;
	}

	/* Prepare things before vfork */
	set_env_vars(pas);

	/* Fork as the user in question and run program */
    //signal(SIGCHLD, SIG_IGN);
	pid = vfork();
	if (pid == 0) {
		/* CHILD */
		/* initgroups, setgid, setuid, and chdir to home or TMPDIR */
		//change_user(pas);
		/*if (DebugOpt) {
			crondlog(LVL5 "child running %s", DEFAULT_SHELL);
		}*/
		/* crond 3.0pl1-100 puts tasks in separate process groups */
//		bb_setpgrp();

        Shakespeare::log(g_fp_log, Shakespeare::NOTICE, PROCESS, "About to execute " + string(line->cl_cmd));
		//execl(DEFAULT_SHELL, DEFAULT_SHELL, "-c", line->cl_cmd, (char *) NULL);
		execl(line->cl_cmd, (char *) NULL);

        char msg[LOG_BUFFER_SIZE];
        sprintf(msg, "can't execute '%s' for user %s", DEFAULT_SHELL, user);
        Shakespeare::log(g_fp_log, Shakespeare::ERROR, PROCESS, string(msg));

        _exit(EXIT_SUCCESS);
	} else if (pid > 0) {
        int status;
		int r = waitpid(line->cl_pid, &status, WNOHANG); // prevent zombies
        if (r == pid) {
                Shakespeare::log(g_fp_log, Shakespeare::NOTICE, PROCESS, "zombie r destroyed = "+r);

                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    process_finished_job(user, line);

                    pid = 0;
                }
        }
    }
	if (pid < 0) {
		/* FORK FAILED */
        char msg[LOG_BUFFER_SIZE];
        sprintf(msg, "can't vfork");
        Shakespeare::log(g_fp_log, Shakespeare::ERROR, PROCESS, string(msg));
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

            	        if (line->cl_failures > MAX_FAILURES) {
                //TODO: Remove magic number
                line->cl_pid = 0;
                continue;
            }
            char failuremsg[LOG_BUFFER_SIZE];
            sprintf(failuremsg,"%s "," pid=");
            sprintf(failuremsg,"%ld ",(long)line->cl_pid);
            sprintf(failuremsg,"%s "," failures=");
            sprintf(failuremsg,"%d ",line->cl_failures);
            sprintf(failuremsg,"%s "," cmd=");
            sprintf(failuremsg,"%s ",line->cl_cmd);

            Shakespeare::log(g_fp_log, Shakespeare::NOTICE, PROCESS, failuremsg);
			
            start_one_job(file->cf_username, line);
			pid = line->cl_pid;

            char pidreturned[LOG_BUFFER_SIZE];
            sprintf(pidreturned,"%s","pid returned = ");
            sprintf(pidreturned,"%3d\n", pid);
            Shakespeare::log(g_fp_log, Shakespeare::WARNING, PROCESS, pidreturned);
            line->cl_time_started = time(NULL);

            char msg[LOG_BUFFER_SIZE];
            sprintf(msg, "USER %s pid %3d cmd %s", file->cf_username, (int)pid, line->cl_cmd);
            Shakespeare::log(g_fp_log, Shakespeare::WARNING, PROCESS, string(msg));

			if (pid < 0) {
				file->cf_wants_starting = 1;
			}
			if (pid > 0) {
				file->cf_has_running = 1;
			}
		}
	}
}

void update_failures_state(CronFile *file, CronLine *line) {
    line->cl_failures += 1;

    if (line->cl_failures > MAX_FAILURES) {
        line->cl_pid = 0;
        // Rollback here
        // line->cl_cmd => /home/apps/current/space-commander/space-commander

        char path[100];
        strcpy(path, line->cl_cmd);

        if (strstr(path, "old/") != NULL){
            strcpy(path, strstr(path, "old/") + 4);
        }else if (strstr(path, "current/") != NULL){
           strcpy(path, strstr(path, "current/") + 8);
        }

        // Removes the filename from the path.
        char* lastSlash = strrchr(path, '/');
        *lastSlash  = '\0';

        Shakespeare::log(g_fp_log, Shakespeare::DEBUG, PROCESS, "path to rollback : " + string(path));

	    chdir("/home/apps/current/space-updater-api");          // Because sockets are created in the current directory.

        UpdaterClient client("sock_rollback");

        client.Connect();

        if (client.Rollback(path) == 0) {
            line->cl_pid = -1;
            line->cl_failures = 0;
		    file->cf_wants_starting = 1;
        }

        client.Disconnect();
    }
    else {
        line->cl_pid = -1;
		file->cf_wants_starting = 1;
    }
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

            //waitpid returned an error or detected a state change in the pid
			if (r < 0 || r == line->cl_pid) {
                // exit(0) detected
                char statusmsg[LOG_BUFFER_SIZE];
                sprintf(statusmsg,"%s ","r=");
                sprintf(statusmsg,"%d ",r);
                sprintf(statusmsg,"%s "," pid=");
                sprintf(statusmsg,"%ld ",(long)line->cl_pid);
                sprintf(statusmsg,"%s "," status=");
                sprintf(statusmsg,"%d ",status);
                Shakespeare::log(g_fp_log, Shakespeare::NOTICE, PROCESS, statusmsg);

                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    Shakespeare::log(g_fp_log, Shakespeare::NOTICE, PROCESS, "exited with status");
                    process_finished_job(file->cf_username, line);

                   if (line->cl_pid == 0) { continue; }
                }

                // crashed or exit (not 0) detected
                update_failures_state(file, line);
                continue;
			}

			/* else: r == 0: "process is still running" */
            if (time(NULL) - line->cl_time_started > MAX_RUN_TIME_IN_SEC) {
                int resultkill = kill(line->cl_pid, SIGKILL);
               // int resultkill = 0;
                char msg[LOG_BUFFER_SIZE];
                sprintf(msg, "kill %d", line->cl_pid);
                Shakespeare::log(g_fp_log, Shakespeare::NOTICE, PROCESS, msg);
                //execl(msg, (char*)NULL);

                sprintf(msg, "failed %3d %3d %d", line->cl_pid, line->cl_failures, resultkill);
                Shakespeare::log(g_fp_log, Shakespeare::ERROR, PROCESS, string(msg));
                update_failures_state(file, line);
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
