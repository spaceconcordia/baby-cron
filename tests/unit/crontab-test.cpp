#include "CppUTest/TestHarness.h"
#include "crontab.h"
#include "config.h"
#include "config-stub.h"
#include <string.h>
#include <algorithm>
#include <sys/stat.h>

#include "tests-helpers.h"
#include "tests-globals.h"

static const int BUFFER_SIZE = 10;
static char zeroOutBuffer[BUFFER_SIZE];
static char user[5] = "root";


TEST_GROUP(CronTab) {
	void setup() {
	    memset(zeroOutBuffer, 0, sizeof(char) * BUFFER_SIZE);
	}
};

TEST(CronTab, ParseField_SingleDigit_BufferHasOneElementSet) {
    char* actual               = zeroOutBuffer; 
    char expected[BUFFER_SIZE] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    char token[2]              = "1"; //Starting from 0, set element 1

    ParseField(user, actual, BUFFER_SIZE, 0, NULL, token);
    CHECK(std::equal(actual, actual + BUFFER_SIZE, expected));
}

TEST(CronTab, ParseField_Hyphen_BufferHasRangeElementsSet) {
    char* actual               = zeroOutBuffer; 
    char expected[BUFFER_SIZE] = {0, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    char token[5]              = "1-5"; //Starting from 0, set elements 1..5

    ParseField(user, actual, BUFFER_SIZE, 0, NULL, token);
    CHECK(std::equal(actual, actual + BUFFER_SIZE, expected));
}

TEST(CronTab, ParseField_Star_BufferHasAllElementsSet) {
    char* actual               = zeroOutBuffer; 
    char expected[BUFFER_SIZE] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    char token[2]              = "*"; //Starting from 0, set all elements

    ParseField(user, actual, BUFFER_SIZE, 0, NULL, token);
    CHECK(std::equal(actual, actual + BUFFER_SIZE, expected));
}

TEST(CronTab, ParseField_Comma_BufferHasListedElementsSet) {
    char* actual               = zeroOutBuffer; 
    char expected[BUFFER_SIZE] = {0, 1, 1, 0, 1, 1, 0, 0, 0, 0};
    char token[8]              = "1,2,4,5"; //Starting from 0, set elements 1,2,4 and 5
    
    ParseField(user, actual, BUFFER_SIZE, 0, NULL, token);
    CHECK(std::equal(actual, actual + BUFFER_SIZE, expected));
}

TEST(CronTab, ParseField_Slash_BufferHasElementsSetIncementally) {
    char* actual               = zeroOutBuffer; 
    char expected[BUFFER_SIZE] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    char token[8] = "0-9/2"; //Starting from 0, set elements 0,2,4,6,8

    ParseField(user, actual, BUFFER_SIZE, 0, NULL, token);
    CHECK(std::equal(actual, actual + BUFFER_SIZE, expected));
}

TEST(CronTab, ParseField_InvalidToken_BufferIsNotChanged) {
    char* actual = zeroOutBuffer;
    char expected[BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char token;
    for(int i = 0; i != 255; i++) {
        token = i;
        //skip digits and *
        if ( (i >= 48 && i <= 57) || i == 42) { continue; }

        ParseField(user, actual, BUFFER_SIZE, 0, NULL, &token);
        CHECK(std::equal(actual, actual + BUFFER_SIZE, expected));
    }
}


TEST(CronTab, LoadCronTab_ValidFile_ReturnCronTabStruct) {
    /* Init memory for dependency injector */
    char *line = (char*)malloc(sizeof(char) * 18);
    strcpy(line, "1 1 1 1 1 command");

    char **tokens;
    tokens    = (char**)malloc(sizeof(char*) * 6);
    tokens[0] = (char*)malloc(sizeof(char) * 2);
    tokens[1] = (char*)malloc(sizeof(char) * 2);
    tokens[2] = (char*)malloc(sizeof(char) * 2);
    tokens[3] = (char*)malloc(sizeof(char) * 2);
    tokens[4] = (char*)malloc(sizeof(char) * 2);
    tokens[5] = (char*)malloc(sizeof(char) * 8);

    strcpy(tokens[0], "1");
    strcpy(tokens[1], "1");
    strcpy(tokens[2], "1");
    strcpy(tokens[3], "1");
    strcpy(tokens[4], "1");
    strcpy(tokens[5], "command");

    /* Inject DI */
    set_config(tokens, line); //No need to free, done internally by config_read

    /* Define expected variables */
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

    /* Start test */
    load_crontab("root");
    CronLine* actual = G.cron_files->cf_lines;

    CHECK(std::equal(actual->cl_Dow,  actual->cl_Dow  + 6,  expectedDow));
    CHECK(std::equal(actual->cl_Mons, actual->cl_Mons + 12, expectedMons));
    CHECK(std::equal(actual->cl_Hrs,  actual->cl_Hrs  + 24, expectedHrs));
    CHECK(std::equal(actual->cl_Days, actual->cl_Days + 31, expectedDays));
    CHECK(std::equal(actual->cl_Mins, actual->cl_Mins + 60, expectedMins));
    STRCMP_EQUAL(actual->cl_cmd, "command");

    for(int i = 0; i != 6; i++) {
        free(tokens[i]);
    }
    free(tokens);
}



TEST(CronTab, FlagStartingPosition_IsNotTimeToStart_ReturnCronLineWithBitsNotSet) {
    FAIL("Do me");
}
