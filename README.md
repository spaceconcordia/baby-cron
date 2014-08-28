# Baby Cron

## Overview

Baby cron is a modified small job scheduler based on the work of the [busybox team](http://www.busybox.net)


## Usage
    - baby-cron will be looking under the '/crontabs' directory
    - crontab files should match a user name (i.e. 'root' for the Q6)
    - crontab files should belong to root:root (even if another user is used)
    - Don't compile with MEMORY LEAK DETECTION 

