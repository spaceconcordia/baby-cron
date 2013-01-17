#include "CppUTest/TestHarness.h"
#include <cstdlib>
#include <string.h>
#include "baby-cron.h"

#include "tests-globals.h"
#include "tests-helpers.h"

static const int BUFFER_SIZE = 10;
static char g_zeroOutBuffer[BUFFER_SIZE];

TEST_GROUP(BabyCron) {
    void setup() {
        memset(g_zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
    }
};

TEST(BabyCron, FlagStartingPosition_IsAboutTimeToStart_ReturnPidEqualsMinusOneAndWantsStartingEqualsOne) {
    // Init DI and inject it
    G.cron_files = (CronFile*)malloc(sizeof(CronFile));
    G.cron_files->cf_deleted = 0;
    G.cron_files->cf_next    = NULL;

    G.cron_files->cf_lines = (CronLine*)malloc(sizeof(CronLine));
    clearAndSet(G.cron_files->cf_lines->cl_Dow,  DOW,  1, 1);
    clearAndSet(G.cron_files->cf_lines->cl_Mons, MONS, 1, 0);
    clearAndSet(G.cron_files->cf_lines->cl_Hrs,  HRS,  1, 1);
    clearAndSet(G.cron_files->cf_lines->cl_Days, DAYS, 1, 1);
    clearAndSet(G.cron_files->cf_lines->cl_Mins, MINS, 1, 1);
    G.cron_files->cf_lines->cl_pid  = 0;
    G.cron_files->cf_lines->cl_next = NULL;

    time_t t1 = time(NULL);
    time_t t2 = t1 + 60;

    // Start test
    flag_starting_jobs(t1, t2);
    
    // Check result
    char expectedPid             = -1;
    char expectedCfWantsStarting = 1;

    char actualPid               = G.cron_files->cf_lines->cl_pid;
    char actualCfWantsStarting   = G.cron_files->cf_wants_starting;

    LONGS_EQUAL(expectedPid, actualPid);
    LONGS_EQUAL(expectedCfWantsStarting, actualCfWantsStarting);

    free(G.cron_files->cf_lines);
    free(G.cron_files);
    freetm(); //Free the memory allocate in the time-stub 
}

TEST(BabyCron, FlagStartingPosition_NotTimeToStart_ReturnPidAndWantsStartingEqualsZero) {
    // Init DI and inject it
    G.cron_files = (CronFile*)malloc(sizeof(CronFile));
    G.cron_files->cf_deleted        = 0;
    G.cron_files->cf_wants_starting = 0;

    G.cron_files->cf_lines = (CronLine*)malloc(sizeof(CronLine));
    
    // The time stub returns everything set to 1, so by putting everything to 2
    // we are in a different time frame
    clearAndSet(G.cron_files->cf_lines->cl_Dow,  DOW,  1, 2);
    clearAndSet(G.cron_files->cf_lines->cl_Mons, MONS, 1, 1);
    clearAndSet(G.cron_files->cf_lines->cl_Hrs,  HRS,  1, 2);
    clearAndSet(G.cron_files->cf_lines->cl_Days, DAYS, 1, 2);
    clearAndSet(G.cron_files->cf_lines->cl_Mins, MINS, 1, 2);
    G.cron_files->cf_lines->cl_pid  = 0;
    G.cron_files->cf_lines->cl_next = NULL;

    time_t t1 = time(NULL);
    time_t t2 = t1 + 60;

    // Start test
    flag_starting_jobs(t1, t2);
    
    // Check result
    char expectedPid             = 0;
    char expectedCfWantsStarting = 0;

    char actualPid               = G.cron_files->cf_lines->cl_pid;
    char actualCfWantsStarting   = G.cron_files->cf_wants_starting;

    LONGS_EQUAL(expectedPid, actualPid);
    LONGS_EQUAL(expectedCfWantsStarting, actualCfWantsStarting);

    free(G.cron_files->cf_lines);
    free(G.cron_files);
    freetm(); //Free the memory allocate in the time-stub 
}
