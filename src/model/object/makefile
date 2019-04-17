# Build the Obj Implementations
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = json_object.o
INCL_DIRS = -Iinclude -I../../ -I../core/include -I../property/include -I/usr/local/include
CORE_DEPS = ../core/include/animation_action.h ../core/include/animation_frame.h ../core/include/animation_graph_handle.h ../core/include/data_related.h
JSON_OBJECT_DEPS = include/json_object.h include/object_3d.h include/object_document.h include/object_frame.h include/object_interface.h include/transforms.h json_object.cpp

default: $(OBJS)

json_object.o: $(CORE_DEPS) $(JSON_OBJECT_DEPS)
	$(CC) $(CFLAGS) -o $@ -c json_object.cpp $(STD) $(INCL_DIRS)
