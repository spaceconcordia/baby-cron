CC=g++
CFLAGS=-Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb
INCPATH=-I./include/
INCTESTPATH=-I./tests/unit/stubs/ -I./tests/helpers/include/
LIBPATH=-L./lib/
LIBS=-lCppUTest -lCppUTestExt

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c -o bin/baby-cron
buildUnitTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) -include./tests/unit/stubs/file-stub.h src/crontab.c tests/unit/stubs/*.cpp tests/unit/*.cpp tests/helpers/src/*.cpp -o bin/AllUnitTests $(LIBS)
buildIntegrationTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) src/*.c tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)
