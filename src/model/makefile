# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = object_document.o animation_property.o
INCL_DIRS = -Iinclude

default: $(OBJS)

animation_property.o: include/data_frameable.h include/data_related.h include/animation_property.h animation_property.cpp
	$(CC) $(CFLAGS) -o $@ -c animation_property.cpp $(STD) $(INCL_DIRS)

object_document.o: include/object_3d.h include/object_document.h include/object_interface.h include/transforms.h object_document.cpp animation_property.o
	$(CC) $(CFLAGS) -o $@ -c object_document.cpp $(STD) $(INCL_DIRS)
