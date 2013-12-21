CC=gcc
MICROCC=microblazeel-xilinx-linux-gnu-g++
MICROCFLAGS=-mcpu=v8.40.b -mxl-barrel-shift -mxl-multiply-high -mxl-pattern-compare -mno-xl-soft-mul -mno-xl-soft-div -mxl-float-sqrt -mhard-float -mxl-float-convert -mlittle-endian -Wall

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
	$(CC) success.c -o success
successQ6:
	$(MICROCC) $(MICROCFLAGS) success.c -o successQ6
segfault:
	$(CC) segfault.c -o segfault
segfaultQ6:
	$(MICROCC) $(MICROCFLAGS) segfault.c -o segfaultQ6

