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

