CC=g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
CFLAGS=-Wall
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb
INCPATH=-I./include/ -I../space-lib/inc/
INCTESTPATH=-I./tests/unit/stubs/ -I./tests/helpers/include/
LIBPATH=-L./lib/ -L../space-lib/lib
LIBS=-lCppUTest -lCppUTestExt
#The test builds have their own main provided by CppUTest so we need to exclude baby-cron-main.c
DEBUG_SRC_FILES =`find src/ ! -name 'baby-cron-main.c' -name '*.c'`

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c -o bin/baby-cron -lshakespeare
buildQ6:
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) $(LIBPATH) src/*.c -o bin/baby-cron -lshakespeare-mbcc

buildAllTests: buildUnitTests buildIntegrationTests
buildUnitTests:
	#Do not include the config.c in the unit tests otherwise it causes multiple defines errors
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) -include./tests/unit/stubs/file-stub.h -include./tests/unit/stubs/time-stub.h src/crontab.c src/baby-cron.c tests/unit/stubs/*.cpp tests/unit/*.cpp tests/helpers/src/*.cpp -o bin/AllUnitTests $(LIBS)
buildIntegrationTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) $(DEBUG_SRC_FILES) tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)
