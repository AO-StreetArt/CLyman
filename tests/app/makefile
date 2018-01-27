# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
TESTS = configuration_test.o log_test.o utils_test.o
BASE_INCL_DIR = $(CURDIR)
INCL_DIRS = -I$(BASE_INCL_DIR)/../../src/app/include -I$(BASE_INCL_DIR)/../../src/api/include -I$(BASE_INCL_DIR)/../../src/model/include

default: $(TESTS)

configuration_test.o: configuration_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) $(INCL_DIRS)

log_test.o: log_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) $(INCL_DIRS)

utils_test.o: utils_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) $(INCL_DIRS)
