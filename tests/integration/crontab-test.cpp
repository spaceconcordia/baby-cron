#include "CppUTest/TestHarness.h"
#include "crontab.h"
#include "config.h"
#include <string.h>
#include <algorithm>
#include <sys/stat.h>

//position starts from 0
void clearAndSet(char* array, int len, char value, int position) {
    memset(array, 0, len);
    *(array + position) = value;
}

//-------------------------------------------------------------------

TEST_GROUP(CronTab) {
	void setup() {
	}
};

TEST(CronTab, LoadCronTab_ValidFileOneLine_ReturnCronTabStruct) {
    CronLine* actual = (CronLine *)malloc(sizeof(CronLine));

    char expectedDow[6]   = {0};
    char expectedMons[12] = {0};
    char expectedHrs[24]  = {0};
    char expectedDays[31] = {0};
    char expectedMins[60] = {0};
 
    load_crontab("crontab-oneline");

    actual = G.cron_files->cf_lines;
    clearAndSet(expectedDow,   6, 1, 1);
    clearAndSet(expectedMons, 12, 1, 0); //Months values have an offset of -1
    clearAndSet(expectedHrs,  24, 1, 1);
    clearAndSet(expectedDays, 31, 1, 1);
    clearAndSet(expectedMins, 60, 1, 1);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + 6,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + 12, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + 24, expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + 31, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + 60, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");
}

TEST(CronTab, LoadCronTab_ValidFileTwoLines_ReturnCronTabStruct) {
    CronLine* actual = (CronLine *)malloc(sizeof(CronLine));

    char expectedDow[6]   = {0};
    char expectedMons[12] = {0};
    char expectedHrs[24]  = {0};
    char expectedDays[31] = {0};
    char expectedMins[60] = {0};
 
    load_crontab("crontab-twolines");

    // First line
    actual = G.cron_files->cf_lines;
    clearAndSet(expectedDow,   6, 1, 1);
    clearAndSet(expectedMons, 12, 1, 0); //Months values have an offset of -1
    clearAndSet(expectedHrs,  24, 1, 1);
    clearAndSet(expectedDays, 31, 1, 1);
    clearAndSet(expectedMins, 60, 1, 1);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + 6,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + 12, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + 24, expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + 31, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + 60, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");

    // Second line
    actual = (G.cron_files->cf_lines->cl_next);
    clearAndSet(expectedDow,   6, 1, 2);
    clearAndSet(expectedMons, 12, 1, 1); //Months values have an offset of -1
    clearAndSet(expectedHrs,  24, 1, 2);
    clearAndSet(expectedDays, 31, 1, 2);
    clearAndSet(expectedMins, 60, 1, 2);

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + 6,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + 12, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + 24, expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + 31, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + 60, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command2");
}


