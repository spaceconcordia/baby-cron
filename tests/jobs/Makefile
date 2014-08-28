#
# compilers
#
CC=gcc
CXX=g++
MICROCC=microblazeel-xilinx-linux-gnu-g++


#
# flags
#
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall

#
# paths
#
CPPUTEST_HOME = ../CppUTest
SPACE_LIB = ../../../space-lib
SPACE_UTLS = $(SPACE_LIB)/utls


#
# rules
#
all: hang fail success segfault
allQ6: hangQ6 failQ6 successQ6 segfaultQ6

hang:
	$(CC) hang.c -o hang
hangQ6:
	$(MICROCC) $(MICROCFLAGS) hang.c -o hangQ6
fail:
	$(CC) fail.c -o fail
failQ6:
	$(MICROCC) $(MICROCFLAGS) fail.c -o failQ6
success:
	$(CXX)  success.c -o success -I$(SPACE_LIB)/include -L$(SPACE_LIB)/shakespeare/lib -L$(SPACE_LIB)/lib -lshakespeare -lcs1_utls
successQ6:
	$(MICROCC) $(MICROCFLAGS) success.c -o successQ6
segfault:
	$(CC) segfault.c -o segfault
segfaultQ6:
	$(MICROCC) $(MICROCFLAGS) segfault.c -o segfaultQ6

#
# clean
#
clean :
	rm -rf hang fail success segfault hangQ6 failQ6 successQ6 segfaultQ6
