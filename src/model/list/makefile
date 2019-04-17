# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = json_object_list.o json_property_list.o
INCL_DIRS = -I$(CURDIR)/../../ -I../core/include -I../property/include -I../object/include -I../factory/include -Iinclude
CORE_LIST_DEPS = include/data_list.h include/data_list_interface.h
OBJECT_LIST_DEPS = include/json_object_list.h include/object_list_interface.h json_object_list.cpp
PROP_LIST_DEPS = include/json_property_list.h include/property_list_interface.h json_property_list.cpp

default: $(OBJS)

json_property_list.o: $(CORE_LIST_DEPS) $(PROP_LIST_DEPS) ../object/json_object.o
	$(CC) $(CFLAGS) -o $@ -c json_property_list.cpp $(STD) $(INCL_DIRS)

json_object_list.o: $(CORE_LIST_DEPS) $(OBJECT_LIST_DEPS) ../property/json_property.o
	$(CC) $(CFLAGS) -o $@ -c json_object_list.cpp $(STD) $(INCL_DIRS)
