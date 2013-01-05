int filenoStub(FILE* f) {
   return 0; 
}

int fstatStub(int no,struct stat *sbuf) {
    sbuf->st_uid = 0;
    return 0;
}
