CC=g++
CFLAGS=-Wall
INCPATH=-I./include/
LIBPATH=-L./lib/
LIBS=-lCppUTest -lCppUTestExt

buildTests:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c tests/*.cpp -o bin/AllTests $(LIBS)
