# Build the Property Implementations
#
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = json_property.o
INCL_DIRS = -Iinclude -I../../ -I../core/include -I/usr/local/include
CORE_PROPERTY_DEPS = ../core/include/animation_action.h ../core/include/animation_frame.h ../core/include/animation_graph_handle.h ../core/include/data_related.h
JSON_PROPERTY_DEPS = include/animation_property.h include/json_property.h include/property_3d.h include/property_frame.h include/property_interface.h json_property.cpp

default: $(OBJS)

json_property.o: $(CORE_PROPERTY_DEPS) $(JSON_PROPERTY_DEPS)
	$(CC) $(CFLAGS) -o $@ -c json_property.cpp $(STD) $(INCL_DIRS)
