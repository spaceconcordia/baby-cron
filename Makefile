CC=g++
CFLAGS=-Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb
INCPATH=-I./include/
INCSTUBPATH=-I./tests/unit/stubs/
LIBPATH=-L./lib/
LIBS=-lCppUTest -lCppUTestExt

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c -o bin/baby-cron
buildUnitTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCSTUBPATH) $(LIBPATH) -include./tests/unit/stubs/file-stub.h src/crontab.c tests/unit/stubs/*.cpp tests/unit/*.cpp -o bin/AllUnitTests $(LIBS)
buildIntegrationTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(LIBPATH) src/config.c src/crontab.c tests/integration/*.cpp -o bin/AllIntegrationTests $(LIBS)
