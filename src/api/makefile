# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = json_object_list.o json_property_list.o
INCL_DIRS = -I$(CURDIR)/../ -Iinclude

default: $(OBJS)

json_property_list.o: include/data_list.h include/data_list_interface.h include/json_property_list.h json_property_list.cpp
	$(CC) $(CFLAGS) -o $@ -c json_property_list.cpp $(STD) $(INCL_DIRS)

json_object_list.o: include/object_list_interface.h include/json_object_list.h json_object_list.cpp json_property_list.o
	$(CC) $(CFLAGS) -o $@ -c json_object_list.cpp $(STD) $(INCL_DIRS)
