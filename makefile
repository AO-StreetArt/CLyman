# Makefile Variables
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
FULL_LIBS = -laossl -lcurl -lpthread -lzmq -llog4cpp -luuid -lmongoc-1.0 -lbson-1.0 -lhiredis `pkg-config --cflags --libs protobuf`
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -Isrc/include
LINK_DIRS = -L/usr/local/lib
RM = rm -f
PROTOC = protoc
PROTO_OPTS = -I=/usr/local/include/dvs_interface
PROTO_OUT = src
OBJS = src/app_log.o src/configuration_manager.o src/globals.o src/transforms.o src/Obj3.pb.cc src/obj3.o src/obj3_list.o main.o
TESTS = configuration_test log_test utils_test transform_test obj3_test obj3_list_test

.PHONY: mksrc mktest

# Central Targets

all: mksrc main.o main

mksrc: src/Obj3.pb.cc
	@$(MAKE) -C src

main.o: main.cpp src/include/app_utils.h
	$(CC) $(CFLAGS) -o $@ -c main.cpp $(STD) $(INCL_DIRS)

main:
	$(CC) $(CFLAGS) -o app $(OBJS) $(FULL_LIBS) $(STD) $(INCL_DIRS) $(LINK_DIRS)

src/Obj3.pb.cc: /usr/local/include/dvs_interface/Obj3.proto
	$(PROTOC) $(PROTO_OPTS) --cpp_out=$(PROTO_OUT) /usr/local/include/dvs_interface/Obj3.proto
	mv $(PROTO_OUT)/Obj3.pb.h src/include/

# Tests

test: mktest $(TESTS)

mktest:
	@$(MAKE) -C tests

configuration_test:
	$(CC) $(CFLAGS) -o $@ src/app_log.o src/configuration_manager.o tests/configuration_test.o $(FULL_LIBS) $(STD) -Isrc/include

log_test:
	$(CC) $(CFLAGS) -o $@ src/app_log.o tests/log_test.o $(FULL_LIBS) $(STD) -Isrc/include

utils_test:
	$(CC) $(CFLAGS) -o $@ tests/utils_test.o $(STD) -Isrc/include

transform_test:
	$(CC) $(CFLAGS) -o $@ tests/transform_test.o src/transforms.o $(STD) -Isrc/include

obj3_test:
	$(CC) $(CFLAGS) -o $@ tests/obj3_test.o src/obj3.o src/transforms.o src/app_log.o $(FULL_LIBS) $(STD) -Isrc/include

obj3_list_test:
	$(CC) $(CFLAGS) -o $@ tests/obj3_list_test.o src/obj3_list.o src/obj3.o src/transforms.o src/app_log.o src/Obj3.pb.cc $(FULL_LIBS) $(STD) -Isrc/include

# Cleanup

clean:
	$(RM) app *.o src/*.o *~ *.log *.log.* *_test tests/*.o src/*.pb.* src/*/*.pb.*
