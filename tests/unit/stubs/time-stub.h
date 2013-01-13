#ifndef TIMESTUB_H
#define TIMESTUB_H 

#include <time.h>

struct tm* localtimeStub(const time_t * timer);
#define localtime(t) localtimeStub(t)

#endif
