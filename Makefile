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
	#Do not include the config.c in the unit tests otherwise it causes multiple defines errors
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) -include./tests/unit/stubs/file-stub.h -include./tests/unit/stubs/time-stub.h src/crontab.c src/baby-cron.c tests/unit/stubs/*.cpp tests/unit/*.cpp tests/helpers/src/*.cpp -o bin/AllUnitTests $(LIBS)
buildIntegrationTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) src/*.c tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)
