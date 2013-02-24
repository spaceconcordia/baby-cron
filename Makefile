CC=g++
MICROCC=microblaze-unknown-linux-gnu-g++
CFLAGS=-Wall
MICROCFLAGS=-mcpu=v8.10.a -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -ffixed-r31 --sysroot /usr/local/lib/mbgcc/microblaze-unknown-linux-gnu/sys-root -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb
INCPATH=-I./include/
INCTESTPATH=-I./tests/unit/stubs/ -I./tests/helpers/include/
LIBPATH=-L./lib/
LIBS=-lCppUTest -lCppUTestExt
#The test builds have their own main provided by CppUTest so we need to exclude baby-cron-main.c
DEBUG_SRC_FILES =`find src/ ! -name 'baby-cron-main.c' -name '*.c'`

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c -o bin/baby-cron
buildQ6:
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) src/*.c -o bin/baby-cron

buildAllTests: buildUnitTests buildIntegrationTests
buildUnitTests:
	#Do not include the config.c in the unit tests otherwise it causes multiple defines errors
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) -include./tests/unit/stubs/file-stub.h -include./tests/unit/stubs/time-stub.h src/crontab.c src/baby-cron.c tests/unit/stubs/*.cpp tests/unit/*.cpp tests/helpers/src/*.cpp -o bin/AllUnitTests $(LIBS)
buildIntegrationTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) $(DEBUG_SRC_FILES) tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)
