CC=g++
MBCC=microblazeel-xilinx-linux-gnu-g++
BB=arm-linux-gnueabi-g++

#
# Paths
#
CPPUTEST_HOME = ../CppUTest
SPACE_LIB = ../space-lib
SPACE_UTLS = $(SPACE_LIB)/utls
UPDATER_API_PATH = ../space-updater-api

#
# Flags
#
CFLAGS=-Wall
CPPFLAGS += -Wall -I$(CPPUTEST_HOME)/include
MEM_LEAK_MACRO = -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h  -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb

#
# includes
#
INCLUDES = -I./include/ -I../space-lib/include/ -I$(UPDATER_API_PATH)/include -I$(CPPUTEST_HOME)/include
INCTESTPATH = -I./tests/unit/stubs/ -I./tests/helpers/include/

#
# Libraries
#
LIBPATH = -L./lib  -L$(SPACE_LIB)/shakespeare/lib -L$(CPPUTEST_HOME)/lib -L$(SPACE_UTLS)/lib
LIBS = -lshakespeare -lcs1_utls
CPPUTEST_LIBS=-lCppUTest -lCppUTestExt 

#The test builds have their own main provided by CppUTest so we need to exclude baby-cron-main.c
DEBUG_SRC_FILES =`find src/ ! -name 'baby-cron-main.c' -name '*.c'`

Q6_TAG=Q6
OBJECTS= bin/UpdaterClient.o bin/baby-cron.o bin/config.o bin/crontab.o
OBJECTS_Q6= bin/UpdaterClient$(Q6_TAG).o

UTEST=$(MEM_LEAK_MACRO) $(CPPUTEST_LIBS) 
ENV= $(UTEST)

make_dir:
	mkdir -p bin lib

buildBin: make_dir bin/baby-cron

bin/baby-cron: src/baby-cron-main.c $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ $^ $(LIBS) $(ENV)

bin/baby-cron.o: src/baby-cron.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(ENV)

bin/config.o: src/config.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(ENV)

bin/crontab.o: src/crontab.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(ENV)




buildQ6: make_dir $(OBJECTS_Q6)
	$(MBCC) $(MICROCFLAGS) $(INCLUDES) $(LIBPATH) src/*.c $(OBJECTS_Q6) -o bin/baby-cron -lshakespeare-mbcc -lcs1_utlsQ6

bin/UpdaterClient.o : $(UPDATER_API_PATH)/src/UpdaterClient.cpp $(UPDATER_API_PATH)/include/UpdaterClient.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(LIBPATH) $(INCLUDES) -c $< -o $@ $(ENV)

bin/UpdaterClientQ6.o : $(UPDATER_API_PATH)/src/UpdaterClient.cpp $(UPDATER_API_PATH)/include/UpdaterClient.h
	$(MBCC) $(CFLAGS) $(DEBUGFLAGS) $(LIBPATH)  $(INCLUDES) -c $< -o $@ $(ENV)

#++++++++++++++++++
# clean
#------------------
clean:
	rm -rf ./bin ./lib



#
# CLEAN UP needed
#
buildBB: make_dir
	$(BB) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) src/*.c $(UPDATER_API_PATH)/bin/UpdaterClient-bb.o -o bin/baby-cron-bb -lshakespeare-BB



#
# Utest are failing....... TODO
#
buildAllTests: make_dir bin/AllUnitTests 
##buildIntegrationTests

#++++++++++++++++++
# UTest 
#------------------
UTEST_INCLUDES= -include ./tests/unit/stubs/file-stub.h -include ./tests/unit/stubs/time-stub.h
UNIT_TEST= tests/unit/baby-cron-test.cpp tests/unit/crontab-test.cpp

# Do not include the config.c in the unit tests otherwise it causes multiple defines errors
bin/AllUnitTests: tests/unit/AllTests.cpp  $(UNIT_TEST) bin/crontab.o bin/baby-cron.o bin/UpdaterClient.o 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCTESTPATH) $(LIBPATH) $(UTEST_INCLUDES) -o $@ tests/unit/stubs/*.cpp  tests/helpers/src/*.cpp  $^  $(LIBS) $(ENV)





buildIntegrationTests: $(OBJECTS) 
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCTESTPATH) $(LIBPATH) $(DEBUG_SRC_FILES) tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)



