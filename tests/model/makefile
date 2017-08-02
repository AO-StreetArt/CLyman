# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
TESTS = transform_test.o obj3_test.o
INCL_DIRS = -I$(CURDIR)/../../src/app/include -I$(CURDIR)/../../src/api/include -I$(CURDIR)/../../src/model/include

default: $(TESTS)

transform_test.o: transform_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) $(INCL_DIRS)

obj3_test.o: obj3_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) $(INCL_DIRS)
