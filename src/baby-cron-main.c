#include <baby-cron.h>
#include <crontab.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <time.h>

#include "shakespeare.h"
#include "SpaceDecl.h"
#include "UpdaterClient.h"

using namespace std; // Yes, we are compiling a C file with g++
const int SLEEP_TIME = 15;

pid_t get_watch_puppy_pid() {
    const int BUFFER_SIZE = 10;
    string filename = CS1_WATCH_PUPPY_PID; 
    char buffer[BUFFER_SIZE] = {0};
    FILE* fp = fopen(filename.c_str(), "r");

    if (fp != NULL) {
        fread(buffer, BUFFER_SIZE, sizeof(char), fp);
        fclose(fp);
        return atoi(buffer);
    }
    else {
        return 0;
    }
}

void signal_watch_puppy() {
    pid_t pid = get_watch_puppy_pid();
    if (pid > 0) {
        kill(pid, SIGUSR1);
    }
}

int main(void) {
    string folder = CS1_LOGS;
    g_fp_log = Shakespeare::open_log(folder, "Baby-Cron");
    Shakespeare::log(g_fp_log, Shakespeare::NOTICE, "Baby-Cron", "Starting");
    fflush(g_fp_log);

	time_t t2;
	INIT_G();

	//xchdir(G.crontab_dir_name);
	//signal(SIGHUP, SIG_IGN); /* ? original crond dies on HUP... */
	//xsetenv("SHELL", DEFAULT_SHELL); /* once, for all future children */
	// crondlog(LVL8 "crond (busybox "BB_VER") started, log level %d", G.log_level);
	rescan_crontab_dir();
	// write_pidfile("/var/run/crond.pid");
	t2 = time(NULL);

	/* Main loop */
	for (;;) {
        #ifdef CS1_DEBUG
            printf(".");fflush(stdout);
        #endif

	 	time_t t1;
	 	long dt;

	 	t1 = t2;

        signal_watch_puppy();

	 	/* Synchronize to 1 minute, minimum 1 second */
	 	// sleep(sleep_time - (time(NULL) % sleep_time) + 1);
        sleep(SLEEP_TIME);

	 	t2 = time(NULL);
	 	dt = (long)t2 - (long)t1;

	 	if (dt > 0) {
	 		/* Usual case: time advances forward, as expected */
	 		flag_starting_jobs(t1, t2);
	 		start_jobs();
	 		if (check_completions() > 0) {
	 			/* some jobs are still running */
	 		}
	 	}
	} /* for (;;) */

    fclose(g_fp_log);
    return 0;
}
