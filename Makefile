CC=g++
CFLAGS=-Wall
DEBUGFLAGS=-ggdb
INCPATH=-I./include/
LIBPATH=-L./lib/
LIBS=-lCppUTest -lCppUTestExt

buildBin:
	$(CC) $(CFLAGS) $(INCPATH) $(LIBPATH) src/*.c -o bin/baby-cron
buildTests:
	$(CC) $(CFLAGS) $(DEBUGFLAGS)  $(INCPATH) $(LIBPATH) src/crontab.c tests/*.cpp -o bin/AllTests $(LIBS)
