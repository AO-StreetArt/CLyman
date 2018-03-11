# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
TESTS = object_list_test.o
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I$(CURDIR)/../../src/app/include -I$(CURDIR)/../../src/api/include -I$(CURDIR)/../../src/model/include

default: $(TESTS)

object_list_test.o: object_list_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) $(INCL_DIRS)
