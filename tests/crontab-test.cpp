#include "CppUTest/TestHarness.h"
#include "crontab.h"
#include <string.h>

static const int BUFFER_SIZE = 10;
static char buffer[BUFFER_SIZE];
static char user[5] = "root";

TEST_GROUP(CronTab) {
	void setup() {
	    memset(buffer, 0, sizeof(char) * BUFFER_SIZE);
	}
};

TEST(CronTab, ParseField_SingleDigit_BufferHasOneElementSet) {
    char token[2]   = "1"; //Starting from 0, set element 1
    ParseField(user, buffer, BUFFER_SIZE, 0, NULL, token);
    CHECK_EQUAL(buffer[1], 1);
}

TEST(CronTab, ParseField_Hyphen_BufferHasRangeElementsSet) {
    char token[5] = "1-5"; //Starting from 0, set elements 1..5
    ParseField(user, buffer, BUFFER_SIZE, 0, NULL, token);
    
    for(int i = 1; i != 5; i++) {
        CHECK_EQUAL(buffer[i], 1);
    }
}

TEST(CronTab, ParseField_Star_BufferHasAllElementsSet) {
    char token[2] = "*"; //Starting from 0, set all elements
    ParseField(user, buffer, BUFFER_SIZE, 0, NULL, token);
    
    for(int i = 0; i != 10; i++) {
        CHECK_EQUAL(buffer[i], 1);
    }
}

TEST(CronTab, ParseField_Comma_BufferHasListedElementsSet) {
    char token[8] = "1,2,4,5"; //Starting from 0, set elements 1,2,4 and 5
    ParseField(user, buffer, BUFFER_SIZE, 0, NULL, token);
    
    CHECK_EQUAL(buffer[1], 1);
    CHECK_EQUAL(buffer[2], 1);
    CHECK_EQUAL(buffer[4], 1);
    CHECK_EQUAL(buffer[5], 1);
}

TEST(CronTab, ParseField_Slash_BufferHasElementsSetIncementally) {
    char token[8] = "0-9/2"; //Starting from 0, set elements 0,2,4,6,8
    ParseField(user, buffer, BUFFER_SIZE, 0, NULL, token);
    
    CHECK_EQUAL(buffer[0], 1);
    CHECK_EQUAL(buffer[2], 1);
    CHECK_EQUAL(buffer[4], 1);
    CHECK_EQUAL(buffer[6], 1);
    CHECK_EQUAL(buffer[8], 1);
}

TEST(CronTab, ParseField_InvalidToken_BufferIsNotChanged) {
    char token;
    for(int i = 0; i != 255; i++) {
        token = i;
        //skip digits and *
        if ( (i >= 48 && i <= 57) || i == 42) { continue; }

        ParseField(user, buffer, BUFFER_SIZE, 0, NULL, &token);
        for(int j = 0; j != BUFFER_SIZE; j++) {
            CHECK_EQUAL(buffer[j], 0);
        }
    }
}
