#include <crontab.h>
#include <baby-cron.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <time.h>
#include "shakespeare.h"
using namespace std;

int main(void) {
    string folder   = "/home/logs/";
    string filename = get_filename(folder, "Baby-Cron.", ".log");
    string filepath = folder + filename;
    g_fp_log = fopen(filepath.c_str(), "a");
    
    Log(g_fp_log, NOTICE, "Baby-Cron", "Starting");
    fflush(g_fp_log);

	time_t t2;
	int sleep_time;

	INIT_G();
	
	//xchdir(G.crontab_dir_name);
	//signal(SIGHUP, SIG_IGN); /* ? original crond dies on HUP... */
	//xsetenv("SHELL", DEFAULT_SHELL); /* once, for all future children */
	// crondlog(LVL8 "crond (busybox "BB_VER") started, log level %d", G.log_level);
	rescan_crontab_dir();
	// write_pidfile("/var/run/crond.pid");

	// /* Main loop */
	t2 = time(NULL);
	sleep_time = 60;
	for (;;) {
	 	time_t t1;
	 	long dt;

	 	t1 = t2;

	 	/* Synchronize to 1 minute, minimum 1 second */
//	 	sleep(sleep_time - (time(NULL) % sleep_time) + 1);
        sleep(sleep_time);

	 	t2 = time(NULL);
	 	dt = (long)t2 - (long)t1;

	 	if (dt > 0) {
	 		/* Usual case: time advances forward, as expected */
	 		flag_starting_jobs(t1, t2);
	 		start_jobs();
	 		if (check_completions() > 0) {
	 			/* some jobs are still running */
	 			sleep_time = 30;
	 		} else {
	 			sleep_time = 30;
	 		}
	 	}
	} /* for (;;) */

    if (g_fp_log) {
        fclose(g_fp_log);
        g_fp_log = NULL;
    }

    return 0;
}
