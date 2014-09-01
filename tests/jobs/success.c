/******************************************************************************
 *
 * COMPILE : g++ success.c -o success -I../../../space-lib/include -L../../../space-lib/shakespeare/lib -L../../../space-lib/lib -lshakespeare -lcs1_utls
 *
 ******************************************************************************/
#include <stdio.h>
#include "shakespeare.h"
int main(void) {
    string folder   = "/home/logs/";
    FILE *g_fp_log = Shakespeare::open_log(folder, "Baby-Cron-TestJob");
    Shakespeare::log(g_fp_log, Shakespeare::NOTICE, "Baby-Cron-TestJob", "tests/jobs/success job executing");
    fflush(g_fp_log);

    return 0;
}
