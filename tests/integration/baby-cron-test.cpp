#include "CppUTest/TestHarness.h"
#include "baby-cron.h"
#include "crontab.h"
#include "config.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

using namespace std;

TEST_GROUP(BabyCron) {
    void setup() {
        set_rtries_for_integration_tests(500000);
        chdir("/home/spaceconcordia/space/baby-cron/tests");
    }

    void teardown() {}
};

TEST(BabyCron, StartJobs_OneJob_ReturnPid) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/one"

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

TEST(BabyCron, StartJobs_FailToStartJob_ReturnPidEqualsMinusOne) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/corrupted"

    INIT_G();
    rescan_crontab_dir();

    //We're cheating, forcing the start
    G.cron_files->cf_wants_starting = 1;
    G.cron_files->cf_lines->cl_pid  = -1;

    start_jobs();

    LONGS_EQUAL(0, G.cron_files->cf_lines->cl_pid);
    LONGS_EQUAL(0, G.cron_files->cf_wants_starting);
}

TEST(BabyCron, StartJobs_FailExitJob_ReturnPidEqualsZeroFailuresEqualsFive) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/fail"

    INIT_G();
    rescan_crontab_dir();

    //We're cheating, forcing the start
    G.cron_files->cf_wants_starting = 1;
    G.cron_files->cf_lines->cl_pid  = -1;

    for(int i = 0; i != 5; i++) {
        start_jobs();
        
        // Sometimes the Linux job scheduler doesn't have the time to 
        // spot that the segfault job has exit hence why we call usleep
        // and recheck again. All this is albitrary.
        check_completions();
        usleep(1);
        check_completions();
        usleep(1);
        check_completions();

        LONGS_EQUAL(-1, G.cron_files->cf_lines->cl_pid);
        LONGS_EQUAL(i + 1, G.cron_files->cf_lines->cl_failures);
    }

    start_jobs();
        
    check_completions();
    usleep(1);
    check_completions();
    usleep(1);
    check_completions();

    LONGS_EQUAL(0, G.cron_files->cf_lines->cl_pid);
    LONGS_EQUAL(5, G.cron_files->cf_lines->cl_failures);
}

TEST(BabyCron, StartJobs_TimeoutJob_ReturnPidEqualsMinusOne) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/hang"

    INIT_G();
    rescan_crontab_dir();

    //We're cheating, forcing the start
    G.cron_files->cf_wants_starting = 1;
    G.cron_files->cf_lines->cl_pid  = -1;

    start_jobs();

    for(int i = 0; i != 10; i++) {
        // TODO: Refactor  
        // Sometimes the Linux job scheduler doesn't have the time to 
        // spot that the segfault job has exit hence why we call usleep
        // and recheck again. All this is albitrary.
        check_completions();
        usleep(1);
        check_completions();
        usleep(1);
        check_completions();

        CHECK(G.cron_files->cf_lines->cl_pid != 0);
    }

    check_completions();
    usleep(1);
    check_completions();
    usleep(1);
    check_completions();

    LONGS_EQUAL(-1, G.cron_files->cf_lines->cl_pid);
    LONGS_EQUAL(1, G.cron_files->cf_lines->cl_failures);
}

TEST(BabyCron, StartJobs_CrashJob_ReturnPidEqualsZero) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/segfault"

    INIT_G();
    rescan_crontab_dir();

    //We're cheating, forcing the start
    G.cron_files->cf_wants_starting = 1;
    G.cron_files->cf_lines->cl_pid  = -1;

    cout << endl << "** Should print 'Segmentation fault (core dumped)' 5 times" << endl;
    for(int i = 0; i != 5; i++) {
        start_jobs();
       
        // TODO: Refactor  
        // Sometimes the Linux job scheduler doesn't have the time to 
        // spot that the segfault job has exit hence why we call usleep
        // and recheck again. All this is albitrary.
        check_completions();
        usleep(1);
        check_completions();
        usleep(1);
        check_completions();

        LONGS_EQUAL(-1, G.cron_files->cf_lines->cl_pid);
        LONGS_EQUAL(i + 1, G.cron_files->cf_lines->cl_failures);
    }

    start_jobs();
        
    check_completions();
    usleep(10);
    check_completions();
    usleep(10);
    check_completions();

    LONGS_EQUAL(0, G.cron_files->cf_lines->cl_pid);
    LONGS_EQUAL(5, G.cron_files->cf_lines->cl_failures);
}
