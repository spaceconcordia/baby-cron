#include "CppUTest/TestHarness.h"
#include "crontab.h"
#include "config.h"
#include <string.h>
#include <algorithm>
#include <sys/stat.h>

static const int BUFFER_SIZE = 10;
static char zeroOutBuffer[BUFFER_SIZE];
static char user[5] = "root";


TEST_GROUP(CronTab) {
	void setup() {
	    memset(zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
	}
};

TEST(CronTab, LoadCronTab_ValidFile_ReturnCronTabStruct) {
    CronLine* actual = (CronLine *)malloc(sizeof(CronLine));
    char expectedDow[6]   = {0, 1, 0, 0, 0, 0};
    char expectedMons[12] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char expectedHrs[24]  = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0};
    char expectedDays[31] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0};
    char expectedMins[60] ={ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    load_crontab("root");

    actual = G.cron_files->cf_lines;
    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + 6,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + 12, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + 24, expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + 31, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + 60, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");



}
