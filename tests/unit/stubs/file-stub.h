#ifndef FILE_STUB
#define FILE_STUB

#include <sys/stat.h>
#include <stdio.h>

int filenoStub(FILE* f);
int fstatStub(int no, struct stat *sbuf);

#define fileno(f) filenoStub(f) 
#define fstat(f, b) fstatStub(f, b)

#endif
