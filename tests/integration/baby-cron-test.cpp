#include "CppUTest/TestHarness.h"
#include "baby-cron.h"
#include "crontab.h"
#include "config.h"
#include <string.h>
#include <sys/stat.h>

TEST_GROUP(BabyCron) {
    void setup() {
        chdir("/home/spaceconcordia/space/baby-cron/tests");
    }

    void teardown() {}
};

TEST(BabyCron, StartJobs_SingleJob_ReturnPid) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/single"

    INIT_G();
    rescan_crontab_dir();

    //We're cheating, forcing the start
    G.cron_files->cf_wants_starting = 1;
    G.cron_files->cf_lines->cl_pid = -1;

    start_jobs();
    CHECK(G.cron_files->cf_lines->cl_pid > 0);
    CHECK(G.cron_files->cf_wants_starting == 0);
    CHECK(G.cron_files->cf_has_running == 1);
}

TEST(BabyCron, StartJobs_TwoJobs_ReturnPid) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/two"

    INIT_G();
    rescan_crontab_dir();

    //We're cheating, forcing the start
    G.cron_files->cf_wants_starting = 1;
    G.cron_files->cf_lines->cl_pid = -1;

    start_jobs();

    // Baby-cron status
    CHECK(G.cron_files->cf_wants_starting == 0);
    CHECK(G.cron_files->cf_has_running == 1); 
    
    // Jobs status
    CHECK(G.cron_files->cf_lines[0].cl_pid > 0);
    CHECK(G.cron_files->cf_lines[1].cl_pid > 0);

}
