# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = app_log.o configuration_manager.o globals.o
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I$(CURDIR)/../api/include/ -I$(CURDIR)/../model/include/ -I$(CURDIR)/include/

default: $(OBJS)

app_log.o: app_log.cpp include/app_log.h
	$(CC) $(CFLAGS) -o $@ -c app_log.cpp $(STD) $(INCL_DIRS)

configuration_manager.o: configuration_manager.cpp include/configuration_manager.h app_log.o
	$(CC) $(CFLAGS) -o $@ -c configuration_manager.cpp $(STD) $(INCL_DIRS)

globals.o: globals.cpp include/globals.h app_log.o configuration_manager.o $(CURDIR)/../api/json_object_list.o $(CURDIR)/../api/protobuf_object_list.o
	$(CC) $(CFLAGS) -o $@ -c globals.cpp $(STD) $(INCL_DIRS)
