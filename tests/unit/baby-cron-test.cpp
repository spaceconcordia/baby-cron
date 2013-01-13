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

TEST(BabyCron, FlagStartingPosition_IsAboutTimeToStart_ReturnCronLineWithBitsSet) {
    char expectedDow[DOW]   = {0};
    char expectedMons[MONS] = {0};
    char expectedHrs[HRS]   = {0};
    char expectedDays[DAYS] = {0};
    char expectedMins[MINS] = {0};

    clearAndSet(expectedDow,   DOW, 1, 1);
    clearAndSet(expectedMons, MONS, 1, 0); //Months values have an offset of -1
    clearAndSet(expectedHrs,   HRS, 1, 1);
    clearAndSet(expectedDays, DAYS, 1, 1);
    clearAndSet(expectedMins, MINS, 1, 1);

    char *actual[6];

    G.cron_files           = (CronFile*)malloc(sizeof(CronFile));
    G.cron_files->cf_lines = (CronLine*)malloc(sizeof(CronLine));

    clearAndSet(G.cron_files->cf_lines->cl_Dow,  DOW,  0, 0);
    clearAndSet(G.cron_files->cf_lines->cl_Mons, MONS, 0, 0);
    clearAndSet(G.cron_files->cf_lines->cl_Hrs,  HRS,  0, 0);
    clearAndSet(G.cron_files->cf_lines->cl_Days, DAYS, 0, 0);
    clearAndSet(G.cron_files->cf_lines->cl_Mins, MINS, 0, 0);

    time_t t1, t2;

    flag_starting_jobs(t1, t2);
    free(G.cron_files->cf_lines);
    free(G.cron_files);
 
    FAIL("Do me");
}
