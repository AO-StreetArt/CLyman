# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = app_log.o configuration_manager.o globals.o transforms.o obj3.o obj3_list.o
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0

default: $(OBJS)

app_log.o: app_log.cpp include/app_log.h
	$(CC) $(CFLAGS) -o $@ -c app_log.cpp $(STD)

configuration_manager.o: configuration_manager.cpp include/configuration_manager.h app_log.o
	$(CC) $(CFLAGS) -o $@ -c configuration_manager.cpp $(STD)

globals.o: globals.cpp include/globals.h app_log.o configuration_manager.o
	$(CC) $(CFLAGS) -o $@ -c globals.cpp $(STD) $(INCL_DIRS)

transforms.o: include/transforms.h transforms.cpp app_log.o
	$(CC) $(CFLAGS) -o $@ -c transforms.cpp $(STD) $(INCL_DIRS)

obj3.o: include/obj3.h obj3.cpp app_log.o transforms.o
	$(CC) $(CFLAGS) -o $@ -c obj3.cpp $(STD) $(INCL_DIRS)

obj3_list.o: include/obj3_list.h obj3_list.cpp app_log.o obj3.o transforms.o
	$(CC) $(CFLAGS) -o $@ -c obj3_list.cpp $(STD) $(INCL_DIRS)
