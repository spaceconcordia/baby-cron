#include "CppUTest/TestHarness.h"
#include "crontab.h"
#include "config.h"
#include <string.h>
#include <algorithm>

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
    FAIL("Implement me!");
}
