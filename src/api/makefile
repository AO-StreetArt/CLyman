# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = json_object_list.o protobuf_object_list.o
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -I$(CURDIR)/../model/include/ -I$(CURDIR)/../app/include/ -Iinclude

default: $(OBJS)

json_object_list.o: include/object_list_interface.h include/object_list.h include/json_object_list.h json_object_list.cpp ../model/object_document.o
	$(CC) $(CFLAGS) -o $@ -c json_object_list.cpp $(STD) $(INCL_DIRS)

protobuf_object_list.o: include/object_list_interface.h include/object_list.h include/protobuf_object_list.h protobuf_object_list.cpp ../model/object_document.o
	$(CC) $(CFLAGS) -o $@ -c protobuf_object_list.cpp $(STD) $(INCL_DIRS)
