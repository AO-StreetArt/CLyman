# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = json_factory.o
INCL_DIRS = -I$(CURDIR)/../../ -I../core/include -I../property/include -I../object/include -I../list/include -Iinclude
COMPILED_DEPS = ../object/json_object.o ../property/json_property.o ../list/json_object_list.o ../list/json_property_list.o
FACTORY_DEPS = include/json_factory.h json_factory.cpp

default: $(OBJS)

json_factory.o: $(FACTORY_DEPS) $(COMPILED_DEPS)
	$(CC) $(CFLAGS) -o $@ -c json_factory.cpp $(STD) $(INCL_DIRS)
