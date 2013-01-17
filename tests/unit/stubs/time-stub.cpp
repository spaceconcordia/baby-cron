#include <cstdlib>
#include <stdio.h>
#include "time-stub.h"

static struct tm *statictm;

struct tm* localtimeStub(const time_t *timer)
{ 
    struct tm *result;

    if (statictm == NULL) {
    	statictm = (tm*)malloc(sizeof(tm));
    }
    result = statictm;

    result->tm_sec   = 1;
    result->tm_min   = 1;
    result->tm_hour  = 1;
    result->tm_mday  = 1;
    result->tm_mon   = 0;
    result->tm_year  = 2013;
    result->tm_wday  = 1;
    result->tm_yday  = 1;
    result->tm_isdst = 1;
    

    return result;
}

void freetm() {
    if (statictm != NULL) {
	    free(statictm);
        statictm = NULL;
    }
}
