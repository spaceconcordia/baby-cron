#include "tests-helpers.h"
#include <cstring>
//position starts from 0
void clearAndSet(char* array, int len, char value, int position) {
    memset(array, 0, len);
    *(array + position) = value;
}

