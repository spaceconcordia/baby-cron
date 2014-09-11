/******************************************************************************
 *
 * COMPILE : g++ success.c -o success -I../../../space-lib/include -L../../../space-lib/shakespeare/lib -L../../../space-lib/lib -lshakespeare -lcs1_utls
 *
 ******************************************************************************/
#include <stdio.h>
#include "shakespeare.h"
int main(void) 
{
    Shakespeare::log_3(Shakespeare::NOTICE, "Baby-Cron-TestJob", "tests/jobs/success job executing");

    return 0;
}
