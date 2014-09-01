CC=g++
MBCC=microblazeel-xilinx-linux-gnu-g++
BB=arm-linux-gnueabi-g++

#
#++++++++++++++++++++
# Paths
#--------------------
CPPUTEST_HOME = ../CppUTest
SPACE_LIB = ../space-lib
SPACE_UTLS = $(SPACE_LIB)/utls
UPDATER_API_PATH = ../space-updater-api

#
#++++++++++++++++++++
# Flags
#--------------------
CFLAGS=
CPPFLAGS += -Wall -I../CppUTest/include
MEM_LEAK_MACRO = -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h \
                    -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul \
              -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall
DEBUGFLAGS=-ggdb -g -gdwarf-2 -g3 #gdwarf-2 + g3 provides macro info to gdb

#
#++++++++++++++++++++
# includes
#--------------------
INCLUDES = -I./include -I$(SPACE_LIB)/include -I$(UPDATER_API_PATH)/include \
                                              -I$(CPPUTEST_HOME)/include
INCLUDE_STUBS = -I./tests/unit/stubs -I./tests/helpers/include

#
#++++++++++++++++++++
# Libraries
#--------------------
LIBPATH = -L./lib  -L$(SPACE_LIB)/shakespeare/lib -L$(CPPUTEST_HOME)/lib \
                                                        -L$(SPACE_UTLS)/lib
LIBS = -lshakespeare -lcs1_utls
CPPUTEST_LIBS=-lCppUTest -lCppUTestExt 

#
# The test builds have their own main provided by CppUTest so we need to exclude baby-cron-main.c
#
DEBUG_SRC_FILES =`find src/ ! -name 'baby-cron-main.c' -name '*.c'`

Q6_TAG=Q6
OBJECTS= bin/UpdaterClient.o bin/baby-cron.o bin/config.o bin/crontab.o
OBJECTS_Q6= bin/UpdaterClient$(Q6_TAG).o

#
# N.B. CppUTest will report 'Deallocating non-allocated memory' when a function like
#      getline() allocate memory that is supposed to be freed in the user program.
#
UTEST_ENV=-DCS1_UTEST $(MEM_LEAK_MACRO) $(CPPUTEST_LIBS) 
ENV= -DCS1_DEBUG 

#
#++++++++++++++++++++
#  x86 
#--------------------
make_dir:
	mkdir -p bin

buildBin: make_dir bin/baby-cron

bin/baby-cron: src/baby-cron-main.c $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ $^ $(LIBS) $(ENV)

bin/baby-cron.o: src/baby-cron.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(ENV)

bin/config.o: src/config.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(ENV)

bin/crontab.o: src/crontab.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(ENV)

bin/UpdaterClient.o : $(UPDATER_API_PATH)/src/UpdaterClient.cpp \
                      $(UPDATER_API_PATH)/include/UpdaterClient.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(LIBPATH) $(INCLUDES) -c $< -o $@ $(ENV)



#
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#   Q6
#------------------------------------------------------------------------------

buildQ6: make_dir $(OBJECTS_Q6)
	$(MBCC) $(MICROCFLAGS) $(INCLUDES) $(LIBPATH) src/*.c $(OBJECTS_Q6) -o bin/baby-cron -lshakespeare-mbcc -lcs1_utlsQ6

bin/UpdaterClientQ6.o : $(UPDATER_API_PATH)/src/UpdaterClient.cpp $(UPDATER_API_PATH)/include/UpdaterClient.h
	$(MBCC) $(CFLAGS) $(DEBUGFLAGS) $(LIBPATH)  $(INCLUDES) -c $< -o $@ $(ENV)

#++++++++++++++++++
# clean
#------------------
clean:
	rm -rf ./bin/* 


#
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# CppUTest  
#------------------------------------------------------------------------------
UTEST_INCLUDES= -include ./tests/unit/stubs/file-stub.h \
                                        -include ./tests/unit/stubs/time-stub.h
UNIT_TEST= tests/unit/crontab-test.cpp # tests/unit/baby-cron-test.cpp 

#
# Utest are failing....... TODO
#
test : make_dir bin/AllTests
##buildIntegrationTests


UTEST_OBJECTS= bin/baby-cronUTEST.o bin/crontabUTEST.o bin/UpdaterClient.o bin/config-stub.o bin/file-stub.o bin/time-stub.o bin/tests-helpers.o


bin/baby-cronUTEST.o: src/baby-cron.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(UTEST_INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)

bin/configUTEST.o: src/config.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(UTEST_INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)

bin/crontabUTEST.o: src/crontab.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(UTEST_INCLUDES) $(LIBPATH)  -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)

#
# tests/unit/stubs
#
bin/config-stub.o : tests/unit/stubs/config-stub.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCLUDE_STUBS) $(LIBPATH) -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)

bin/file-stub.o : tests/unit/stubs/file-stub.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCLUDE_STUBS) $(LIBPATH) -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)

bin/time-stub.o : tests/unit/stubs/time-stub.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCLUDE_STUBS) $(LIBPATH)   -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)

#
# tests/helpers
#
bin/tests-helpers.o : tests/helpers/src/tests-helpers.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCLUDE_STUBS) $(LIBPATH)   -o $@ -c $^ $(LIBS) $(UTEST_ENV) $(ENV)


#
# AllTests
#
bin/AllTests: tests/unit/AllTests.cpp  $(UNIT_TEST) $(UTEST_OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCLUDE_STUBS) $(LIBPATH) \
                                             -o $@  $^ $(LIBS)  $(ENV) $(UTEST_ENV) 


#
#
#
#
#
#
# <=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-===--- * * * * 
# CLEAN UP needed
#
buildBB: make_dir
	$(BB) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(LIBPATH) src/*.c $(UPDATER_API_PATH)/bin/UpdaterClient-bb.o -o bin/baby-cron-bb -lshakespeare-BB





buildIntegrationTests: $(OBJECTS) 
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) $(INCLUDE_STUBS) $(LIBPATH) $(DEBUG_SRC_FILES) tests/integration/*.cpp tests/helpers/src/*.cpp -o bin/AllIntegrationTests $(LIBS)



