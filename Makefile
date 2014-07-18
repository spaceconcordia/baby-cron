CC=g++
MICROCC=microblazeel-xilinx-linux-gnu-g++
BB=arm-linux-gnueabi-g++

CFLAGS=-Wall
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb

UPDATER_API_PATH = ../space-updater-api
INCPATH=-I./include/ -I../space-lib/include/ -I$(UPDATER_API_PATH)/include
INCTESTPATH=-I./tests/unit/stubs/ -I./tests/helpers/include/
LIBPATH=-L./lib/ -L../space-lib/lib/
LIBS=-lCppUTest -lCppUTestExt -lshakespeare

#The test builds have their own main provided by CppUTest so we need to exclude baby-cron-main.c
DEBUG_SRC_FILES =`find src/ ! -name 'baby-cron-main.c' -name '*.c'`

Q6_TAG=Q6
OBJECTS= bin/UpdaterClient.o
OBJECTS_Q6= bin/UpdaterClient$(Q6_TAG).o

make_dir:
	mkdir -p bin lib

buildBin: make_dir $(OBJECTS)
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c $(OBJECTS) -o bin/baby-cron -lshakespeare

buildQ6: make_dir $(OBJECTS_Q6)
	$(MICROCC) $(MICROCFLAGS) $(INCPATH) $(LIBPATH) src/*.c $(OBJECTS_Q6) -o bin/baby-cron -lshakespeare-mbcc

bin/UpdaterClient.o : $(UPDATER_API_PATH)/src/UpdaterClient.cpp $(UPDATER_API_PATH)/include/UpdaterClient.h
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) -c $< -o $@

bin/UpdaterClientQ6.o : $(UPDATER_API_PATH)/src/UpdaterClient.cpp $(UPDATER_API_PATH)/include/UpdaterClient.h
	$(MICROCC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) -c $< -o $@

#++++++++++++++++++
# clean
#------------------
clean:
	rm -rf ./bin



#
# CLEAN UP needed
#
buildBB: make_dir
	$(BB) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c $(UPDATER_API_PATH)/bin/UpdaterClient-bb.o -o bin/baby-cron-bb -lshakespeare-BB



buildAllTests: buildUnitTests buildIntegrationTests
buildUnitTests:
	#Do not include the config.c in the unit tests otherwise it causes multiple defines errors
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) -include./tests/unit/stubs/file-stub.h -include./tests/unit/stubs/time-stub.h src/crontab.c src/baby-cron.c tests/unit/stubs/*.cpp tests/unit/*.cpp tests/helpers/src/*.cpp $(UPDATER_API_PATH)/bin/UpdaterClient.o -o bin/AllUnitTests $(LIBS)
buildIntegrationTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) $(INCTESTPATH) $(LIBPATH) $(DEBUG_SRC_FILES) tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)



