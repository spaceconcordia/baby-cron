/*
* Note: Make sure the owner of the crontab files is root or tests will
* randomly crash or seg. fault.
*/

#include "CppUTest/TestHarness.h"
#include "baby-cron.h"
#include "crontab.h"
#include "config.h"
#include <string.h>
#include <algorithm>
#include <sys/stat.h>

#include "tests-helpers.h"
#include "tests-globals.h"


//-------------------------------------------------------------------

TEST_GROUP(CronTab) {
	void setup() {
        chdir("/home/spaceconcordia/space/baby-cron/tests");
	}
};

TEST(CronTab, LoadCronTab_ValidFileOneLine_ReturnCronTabStruct) {
    CronLine* actual = (CronLine *)malloc(sizeof(CronLine));

    char expectedDow[DOW]   = {0};
    char expectedMons[MONS] = {0};
    char expectedHrs[HRS]   = {0};
    char expectedDays[DAYS] = {0};
    char expectedMins[MINS] = {0};
 
    load_crontab("crontab-oneline");

    actual = G.cron_files->cf_lines;
    clearAndSet(expectedDow,   DOW, 1, 1);
    clearAndSet(expectedMons, MONS, 1, 0); //Months values have an offset of -1
    clearAndSet(expectedHrs,   HRS, 1, 1);
    clearAndSet(expectedDays, DAYS, 1, 1);
    clearAndSet(expectedMins, MINS, 1, 1);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + DOW,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + MONS, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + HRS,  expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + DAYS, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + MINS, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");
}

TEST(CronTab, LoadCronTab_ValidFileTwoLines_ReturnCronTabStruct) {
    CronLine* actual = (CronLine *)malloc(sizeof(CronLine));

    char expectedDow[DOW]   = {0};
    char expectedMons[MONS] = {0};
    char expectedHrs[HRS]   = {0};
    char expectedDays[DAYS] = {0};
    char expectedMins[MINS] = {0};

    load_crontab("crontab-twolines");

    // First line
    actual = G.cron_files->cf_lines;
    clearAndSet(expectedDow,   DOW, 1, 1);
    clearAndSet(expectedMons, MONS, 1, 0); //Months values have an offset of -1
    clearAndSet(expectedHrs,   HRS, 1, 1);
    clearAndSet(expectedDays, DAYS, 1, 1);
    clearAndSet(expectedMins, MINS, 1, 1);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + DOW,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + MONS, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + HRS,  expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + DAYS, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + MINS, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");

    // Second line
    actual = (G.cron_files->cf_lines->cl_next);
    clearAndSet(expectedDow,   DOW, 1, 2);
    clearAndSet(expectedMons, MONS, 1, 1); //Months values have an offset of -1
    clearAndSet(expectedHrs,   HRS, 1, 2);
    clearAndSet(expectedDays, DAYS, 1, 2);
    clearAndSet(expectedMins, MINS, 1, 2); 
 
    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + DOW,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + MONS, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + HRS,  expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + DAYS, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + MINS, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command2");
}

TEST(CronTab, LoadCronTab_FileWithOneInvalidLine_ReturnCronTabStruct) {
    CronLine* actual = (CronLine *)malloc(sizeof(CronLine));

    char expectedDow[DOW]   = {0};
    char expectedMons[MONS] = {0};
    char expectedHrs[HRS]   = {0};
    char expectedDays[DAYS] = {0};
    char expectedMins[MINS] = {0};

    load_crontab("crontab-valid-invalid-valid-lines");

    // First line
    actual = G.cron_files->cf_lines;
    clearAndSet(expectedDow,   DOW, 1, 1);
    clearAndSet(expectedMons, MONS, 1, 0); //Months values have an offset of -1
    clearAndSet(expectedHrs,   HRS, 1, 1);
    clearAndSet(expectedDays, DAYS, 1, 1);
    clearAndSet(expectedMins, MINS, 1, 1);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + DOW,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + MONS, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + HRS,  expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + DAYS, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + MINS, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");

    // Second line
    actual = (G.cron_files->cf_lines->cl_next);
    bzero(expectedDow,  DOW);
    bzero(expectedMons, MONS); 
    bzero(expectedHrs,  HRS);
    bzero(expectedDays, DAYS);
    bzero(expectedMins, MINS);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + DOW,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + MONS, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + HRS,  expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + DAYS, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + MINS, expectedMins));

    //Command is still read correctly, even though it's useless
    STRCMP_EQUAL(actual->cl_cmd, "nope"); 

    // Third line
    actual = (G.cron_files->cf_lines->cl_next->cl_next);
    clearAndSet(expectedDow,   DOW, 1, 2);
    clearAndSet(expectedMons, MONS, 1, 1); //Months values have an offset of -1
    clearAndSet(expectedHrs,   HRS, 1, 2);
    clearAndSet(expectedDays, DAYS, 1, 2);
    clearAndSet(expectedMins, MINS, 1, 2);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + DOW,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + MONS, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + HRS,  expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + DAYS, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + MINS, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command2");
}

TEST(CronTab, RescanCronTabDir_FileFound_ReturnCronFileStruct) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs"    

    INIT_G();
    rescan_crontab_dir();
    CHECK(G.cron_files != NULL);
    CHECK(G.cron_files->cf_lines != NULL);
    STRCMP_EQUAL(G.cron_files->cf_username, "babycronjobs");
}

TEST(CronTab, RescanCronTabDir_FileNotFound_DoesNotReturnCronFileStruct) {
    #undef CRONTABS
    #define CRONTABS "/home/spaceconcordia/space/baby-cron/tests/crontabs/empty"

    INIT_G();
    rescan_crontab_dir();
    CHECK(G.cron_files == NULL);
}

