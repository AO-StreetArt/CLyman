# Makefile Variables
CC = g++
CFLAGS  = -g -Wall
STD = -std=c++11
FULL_LIBS = -laossl -lcurl -lpthread -lzmq -llog4cpp -luuid -lmongoc-1.0 -lbson-1.0 -lrdkafka -lcppkafka `pkg-config --cflags --libs protobuf`
INCL_DIRS = -I/usr/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/libbson-1.0 -Isrc/api/include -Isrc/app/include -Isrc/model/include
BASE_INCL_DIR = $(CURDIR)
LINK_DIRS = -L/usr/local/lib
RM = rm -f
PROTOC = protoc
PROTO_OPTS = -I=/usr/local/include/dvs_interface
PROTO_OUT = src/api/
OBJS = src/app/app_log.o src/app/configuration_manager.o src/app/globals.o src/model/object_document.o src/api/Obj3.pb.cc src/api/json_object_list.o src/api/protobuf_object_list.o main.o
TESTS = configuration_test log_test utils_test transform_test obj3_test object_list_test

.PHONY: mksrc mktest

# Central Targets

all: mksrc main.o main

mksrc: src/Obj3.pb.cc
	@$(MAKE) -C src

main.o: main.cpp src/app/include/app_utils.h
	$(CC) $(CFLAGS) -o $@ -c main.cpp $(STD) $(INCL_DIRS)

main:
	$(CC) $(CFLAGS) -o clyman $(OBJS) $(FULL_LIBS) $(STD) $(INCL_DIRS) $(LINK_DIRS)

src/Obj3.pb.cc: /usr/local/include/dvs_interface/Obj3.proto
	$(PROTOC) $(PROTO_OPTS) --cpp_out=$(PROTO_OUT) /usr/local/include/dvs_interface/Obj3.proto
	mv $(PROTO_OUT)/Obj3.pb.h src/api/include/

# Tests

test: mktest $(TESTS)
	./configuration_test -config-file=tests/test.properties
	./utils_test
	./log_test
	./transform_test
	./obj3_test
	./object_list_test

mktest:
	@$(MAKE) -C tests

configuration_test:
	$(CC) $(CFLAGS) -o $@ src/app/app_log.o src/app/configuration_manager.o tests/app/configuration_test.o $(FULL_LIBS) $(STD) $(INCL_DIRS)

log_test:
	$(CC) $(CFLAGS) -o $@ src/app/app_log.o tests/app/log_test.o $(FULL_LIBS) $(STD) $(INCL_DIRS)

utils_test:
	$(CC) $(CFLAGS) -o $@ tests/app/utils_test.o $(STD) $(INCL_DIRS)

transform_test:
	$(CC) $(CFLAGS) -o $@ tests/model/transform_test.o $(STD) $(INCL_DIRS)

obj3_test:
	$(CC) $(CFLAGS) -o $@ tests/model/obj3_test.o src/model/object_document.o src/app/app_log.o $(FULL_LIBS) $(STD) $(INCL_DIRS)

object_list_test:
	$(CC) $(CFLAGS) -o $@ tests/api/object_list_test.o src/api/protobuf_object_list.o src/api/json_object_list.o src/model/object_document.o src/app/app_log.o src/api/Obj3.pb.cc $(FULL_LIBS) $(STD) $(INCL_DIRS)

# Cleanup

clean:
	$(RM) app clyman *.o src/*.o *~ *.log *.log.* *_test tests/*.o src/*/*.pb.* src/*/*/*.pb.* src/*/*.o tests/*/*.o
