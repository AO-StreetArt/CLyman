# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
TESTS = configuration_test.o log_test.o utils_test.o transform_test.o obj3_test.o obj3_list_test.o

default: $(TESTS)

configuration_test.o: configuration_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) -I../src/include

log_test.o: log_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) -I../src/include

utils_test.o: utils_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) -I../src/include

transform_test.o: transform_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) -I../src/include

obj3_test.o: obj3_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) -I../src/include

obj3_list_test.o: obj3_list_test.cpp
	$(CC) $(CFLAGS) -o $@ -c $^ $(STD) -I../src/include
