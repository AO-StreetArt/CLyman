# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = object_document.o
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I$(CURDIR)/../api/include/ -I$(CURDIR)/../app/include/

default: $(OBJS)

object_document.o: include/object_3d.h include/object_related.h include/object_document.h include/object_interface.h include/transforms.h object_document.cpp
	$(CC) $(CFLAGS) -o $@ -c object_document.cpp $(STD) $(INCL_DIRS)
