#include <cstdlib>
#include "time-stub.h"

struct tm* localtimeStub(const time_t *timer)
{
    struct tm *result = (tm*)malloc(sizeof(tm));
    result->tm_sec   = 1;
    result->tm_min   = 1;
    result->tm_hour  = 1;
    result->tm_mday  = 1;
    result->tm_mon   = 1;
    result->tm_year  = 2013;
    result->tm_wday  = 1;
    result->tm_yday  = 1;
    result->tm_isdst = 1;
    
    return result;
}
